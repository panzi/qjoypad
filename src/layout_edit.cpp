#include "layout_edit.h"
#include "config.h"

//build the dialog
LayoutEdit::LayoutEdit( LayoutManager* l )
    : QWidget(0),
      lm(l),
      mainLayout(0),
      padStack(0),
      joyButtons(0),
      cmbLayouts(0),
      btnAdd(0),
      btnRem(0),
      btnUpd(0),
      btnRev(0),
      btnExport(0),
      btnImport(0),
      btnRename(0) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle( QJOYPAD_NAME );
    setWindowIcon(QPixmap(QJOYPAD_ICON64));

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);

    QFrame* frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Sunken);
    QGridLayout* g = new QGridLayout(frame);
    g->setMargin(5);
    g->setSpacing(5);
    cmbLayouts = new QComboBox(frame);
    connect(cmbLayouts, SIGNAL(activated(int)), this, SLOT(load(int)));

    //most of these buttons can link directly into slots in the LayoutManager
    btnAdd = new QPushButton(frame);
    btnAdd->setIcon(QIcon::fromTheme("list-add"));
    btnAdd->setToolTip(tr("Add Layout"));
    if (btnAdd->icon().isNull()) {
        btnAdd->setText("+");
    }
    btnAdd->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(btnAdd, SIGNAL(clicked()), lm, SLOT(saveAs()));

    btnRem = new QPushButton(frame);
    btnRem->setIcon(QIcon::fromTheme("list-remove"));
    if (btnRem->icon().isNull()) {
        btnRem->setText("-");
    }
    btnRem->setToolTip(tr("Remove Layout"));
    btnRem->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(btnRem, SIGNAL(clicked()), lm, SLOT(remove()));

    btnRename = new QPushButton(tr("&Rename"), frame);
    btnRename->setToolTip(tr("Rename Layout"));
    btnRename->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(btnRename, SIGNAL(clicked()), lm, SLOT(rename()));

    QHBoxLayout *layoutLayout = new QHBoxLayout();
    layoutLayout->addWidget(cmbLayouts);
    layoutLayout->addWidget(btnAdd);
    layoutLayout->addWidget(btnRem);
    layoutLayout->addWidget(btnRename);
    mainLayout->addLayout(layoutLayout);

    btnImport = new QPushButton(QIcon::fromTheme("document-open"), tr("&Import"), frame);
    connect(btnImport, SIGNAL(clicked()), lm, SLOT(importLayout()));
    g->addWidget(btnImport,1,0);

    btnExport = new QPushButton(QIcon::fromTheme("document-save-as"), tr("E&xport"), frame);
    connect(btnExport, SIGNAL(clicked()), lm, SLOT(exportLayout()));
    g->addWidget(btnExport,1,1);

    btnUpd = new QPushButton(QIcon::fromTheme("document-save"), tr("&Save"), frame);
    connect(btnUpd, SIGNAL(clicked()), lm, SLOT(save()));
    g->addWidget(btnUpd,1,2);

    btnRev = new QPushButton(QIcon::fromTheme("document-revert"), tr("Re&vert"), frame);
    connect(btnRev, SIGNAL(clicked()), lm, SLOT(reload()));
    g->addWidget(btnRev,1,3);
    mainLayout->addWidget( frame );

    //produce a list of names for the FlashRadioArray
    //this is only necesary since joystick devices need not always be
    //contiguous
    QStringList names;
    foreach (JoyPad *joypad, lm->available) {
        names.append(joypad->getName());
        connect(this, SIGNAL(focusStateChanged(bool)), joypad, SLOT(focusChange(bool)));
    }
    
    //flash radio array
    joyButtons = new FlashRadioArray(names, true, this );
    mainLayout->addWidget( joyButtons );
    
    //we have a WidgetStack to represent the multiple joypads
    padScroll = new QScrollArea(this);
    padScroll->setWidgetResizable(true);
    mainLayout->addWidget(padScroll);
    padStack = new QStackedWidget( this );
    padStack->setFrameStyle(QFrame::Box | QFrame::Sunken );
    padScroll->setWidget(padStack);

    //go through each of the available joysticks
    // i is the current index into PadStack
    int i = 0;
    foreach (JoyPad *joypad, lm->available) {
        //add a new JoyPadWidget to the stack
        padStack->insertWidget( i, joypad->widget(padStack,i) );
        //every time it "flashes", flash the associated tab.
        connect( padStack->widget(i), SIGNAL( flashed( int ) ), joyButtons, SLOT( flash( int )));
        ++i;
    }
    //whenever a new tab is selected, raise the appropriate JoyPadWidget
    connect( joyButtons, SIGNAL( changed( int ) ), padStack, SLOT( setCurrentIndex( int )));

    updateLayoutList();

    //add the buttons at the bottom.
    QPushButton* close = new QPushButton(QIcon::fromTheme("window-close"), tr("&Close Dialog"), this );
    connect(close, SIGNAL(clicked()), this, SLOT(close()));

    QPushButton* quit = new QPushButton(QIcon::fromTheme("application-exit"), tr("&Quit"), this );
    connect( quit, SIGNAL( clicked() ), qApp, SLOT(quit()));

    QHBoxLayout* h = new QHBoxLayout();
    h->setMargin(0);
    h->setSpacing(5);
    h->addWidget(close);
    h->addWidget(quit);
    mainLayout->addLayout(h);

    connect(qApp, SIGNAL(focusChanged ( QWidget * , QWidget *  ) ), this, 
        SLOT(appFocusChanged(QWidget *, QWidget *)));

    this->show();
}

void LayoutEdit::setLayout(const QString &layout) {
    //change the selection
    for (int i = 0; i < cmbLayouts->count(); ++ i) {
        if (cmbLayouts->itemData(i).toString() == layout) {
            cmbLayouts->setCurrentIndex(i);
            break;
        }
    }

    bool hasLayout = !layout.isNull();
    btnRem->setEnabled(hasLayout);
    btnRename->setEnabled(hasLayout);

    //update all the JoyPadWidgets.
    for (int i = 0, n = lm->available.count(); i < n; i++) {
        ((JoyPadWidget*)padStack->widget(i))->update();
    }
}

void LayoutEdit::updateLayoutList() {
    //blank the list, then load in new names from the LayoutManager.
    cmbLayouts->clear();
    cmbLayouts->addItem(tr("[NO LAYOUT]"), QVariant(QString()));
    if (lm->currentLayout.isNull()) {
        cmbLayouts->setCurrentIndex(0);
    }
    foreach (const QString& layout, lm->getLayoutNames()) {
        cmbLayouts->addItem(layout, layout);
        if (layout == lm->currentLayout) {
            cmbLayouts->setCurrentIndex(cmbLayouts->count() - 1);
        }
    }
}

void LayoutEdit::updateJoypadWidgets() {
    int indexOfFlashRadio = mainLayout->indexOf(joyButtons);
    FlashRadioArray *newJoyButtons;
    QStringList names;
    foreach (JoyPad *joypad, lm->available) {
        names.append(joypad->getName());
    }
    
    newJoyButtons = new FlashRadioArray( names, true, this );
    mainLayout->insertWidget(indexOfFlashRadio, newJoyButtons);
    mainLayout->removeWidget(joyButtons);
    FlashRadioArray* oldJoyButtons = joyButtons;
    joyButtons = newJoyButtons;
    connect( joyButtons, SIGNAL( changed( int ) ), padStack, SLOT( setCurrentIndex( int )));
    oldJoyButtons->deleteLater();
    int numberOfJoypads = padStack->count();
    for(int i = 0; i<numberOfJoypads; i++) {
        padStack->removeWidget(padStack->widget(0));
    }
    int i = 0;
    foreach (JoyPad *joypad, lm->available) {
        //add a new JoyPadWidget to the stack
        padStack->insertWidget( i, joypad->widget(padStack,i) );
        //every time it "flashes", flash the associated tab.
        connect( padStack->widget(i), SIGNAL( flashed( int ) ), joyButtons, SLOT( flash( int )));
        ++i;
    }
}

void LayoutEdit::appFocusChanged(QWidget *old, QWidget *now) {
    if (now != NULL && old == NULL) {
        emit focusStateChanged(false);
    } else if(old != NULL && now == NULL) {
        emit focusStateChanged(true);
        foreach (JoyPad *joypad, lm->available) {
            debug_mesg("iterating and releasing\n");
            joypad->release();
        }
        debug_mesg("done releasing!\n");
    }
}

void LayoutEdit::load(int index) {
    lm->load(cmbLayouts->itemData(index).toString());
}
