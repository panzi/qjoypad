/****************************************************************************
** TrayIcon meta object code from reading C++ file 'trayicon.h'
**
** Created: Mon May 31 10:09:03 2004
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.1.2   edited Feb 24 09:29 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "trayicon.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.1.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#include <qvariant.h>
const char *TrayIcon::className() const
{
    return "TrayIcon";
}

QMetaObject *TrayIcon::metaObj = 0;
static QMetaObjectCleanUp cleanUp_TrayIcon( "TrayIcon", &TrayIcon::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString TrayIcon::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TrayIcon", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString TrayIcon::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TrayIcon", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* TrayIcon::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "icon", &static_QUType_varptr, "\x06", QUParameter::In }
    };
    static const QUMethod slot_0 = {"setIcon", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "tip", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"setToolTip", 1, param_slot_1 };
    static const QUMethod slot_2 = {"show", 0, 0 };
    static const QUMethod slot_3 = {"hide", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "setIcon(const QPixmap&)", &slot_0, QMetaData::Public },
	{ "setToolTip(const QString&)", &slot_1, QMetaData::Public },
	{ "show()", &slot_2, QMetaData::Public },
	{ "hide()", &slot_3, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"clicked", 2, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod signal_1 = {"doubleClicked", 1, param_signal_1 };
    static const QUMethod signal_2 = {"closed", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "clicked(const QPoint&,int)", &signal_0, QMetaData::Public },
	{ "doubleClicked(const QPoint&)", &signal_1, QMetaData::Public },
	{ "closed()", &signal_2, QMetaData::Public }
    };
#ifndef QT_NO_PROPERTIES
    static const QMetaProperty props_tbl[2] = {
 	{ "QString","toolTip", 0x3000103, &TrayIcon::metaObj, 0, -1 },
	{ "QPixmap","icon", 0x6000103, &TrayIcon::metaObj, 0, -1 }
    };
#endif // QT_NO_PROPERTIES
    metaObj = QMetaObject::new_metaobject(
	"TrayIcon", parentObject,
	slot_tbl, 4,
	signal_tbl, 3,
#ifndef QT_NO_PROPERTIES
	props_tbl, 2,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_TrayIcon.setMetaObject( metaObj );
    return metaObj;
}

void* TrayIcon::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "TrayIcon" ) )
	return this;
    return QObject::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL clicked
void TrayIcon::clicked( const QPoint& t0, int t1 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[3];
    static_QUType_varptr.set(o+1,&t0);
    static_QUType_int.set(o+2,t1);
    activate_signal( clist, o );
}

// SIGNAL doubleClicked
void TrayIcon::doubleClicked( const QPoint& t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_varptr.set(o+1,&t0);
    activate_signal( clist, o );
}

// SIGNAL closed
void TrayIcon::closed()
{
    activate_signal( staticMetaObject()->signalOffset() + 2 );
}

bool TrayIcon::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setIcon((const QPixmap&)*((const QPixmap*)static_QUType_ptr.get(_o+1))); break;
    case 1: setToolTip((const QString&)static_QUType_QString.get(_o+1)); break;
    case 2: show(); break;
    case 3: hide(); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool TrayIcon::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: clicked((const QPoint&)*((const QPoint*)static_QUType_ptr.get(_o+1)),(int)static_QUType_int.get(_o+2)); break;
    case 1: doubleClicked((const QPoint&)*((const QPoint*)static_QUType_ptr.get(_o+1))); break;
    case 2: closed(); break;
    default:
	return QObject::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool TrayIcon::qt_property( int id, int f, QVariant* v)
{
    switch ( id - staticMetaObject()->propertyOffset() ) {
    case 0: switch( f ) {
	case 0: setToolTip(v->asString()); break;
	case 1: *v = QVariant( this->toolTip() ); break;
	case 3: case 4: case 5: break;
	default: return FALSE;
    } break;
    case 1: switch( f ) {
	case 0: setIcon(v->asPixmap()); break;
	case 1: *v = QVariant( this->icon() ); break;
	case 3: case 4: case 5: break;
	default: return FALSE;
    } break;
    default:
	return QObject::qt_property( id, f, v );
    }
    return TRUE;
}

bool TrayIcon::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
