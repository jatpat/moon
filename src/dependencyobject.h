/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * dependencyobject.h: 
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 * 
 */

#ifndef __MONO_DEPOBJECT_H__
#define __MONO_DEPOBJECT_H__

#include <glib.h>

#include "provider.h"
#include "dependencyproperty.h"
#include "value.h"
#include "enums.h"
#include "list.h"

#define EVENTHANDLER(type, event, objtype, argtype)	\
	static void event##Callback (EventObject *sender, EventArgs *calldata, gpointer closure)	\
	{	\
		g_return_if_fail (sender != NULL);	\
		((type *) closure)->event##Handler ((objtype *) sender, (argtype *) calldata); \
	}	\
	void event##Handler (objtype *sender, argtype *calldata);

class CollectionChangedEventArgs;
class EventObject;
class EventArgs;
struct EmitContext;
class MoonError;

typedef void (* TickCallHandler) (EventObject *object);
typedef void (* EventHandler) (EventObject *sender, EventArgs *args, gpointer closure);
typedef bool (* EventHandlerPredicate) (EventHandler cb_handler, gpointer cb_data, gpointer data);

class EventLists;

// 
// An EventObject starts out with a reference count of 1 (no need to
// ref it after creation), and will be deleted once the count reaches
// 0.
//
// DEBUGGING
// 
// To log all creation/destruction/ref/unref of an object,
// define OBJECT_TRACK_ID to that object's in dependencyobject.cpp.
// (this will require that you first run the program once to print
// the id of the object you're interested in).
//

#define GET_OBJ_ID(x) (x ? x->GetId () : 0)

typedef void (* ToggleNotifyHandler) (EventObject *sender, bool isLastRef);
class ToggleNotifyListener {
public:
	ToggleNotifyListener (EventObject *sender, ToggleNotifyHandler callback)
	{
		this->callback = callback;
		this->sender = sender;
	}

	virtual void Invoke (bool isLastRef)
	{
		callback (sender, isLastRef);
	}

private:
	EventObject *sender;
	ToggleNotifyHandler callback;
};

class EventObject {
private:
	enum Flags {
		MultiThreadedSafe = 1 << 29, // if the dtor can be called on any thread
		Attached = 1 << 30,
		Disposed = 1 << 31,
		IdMask = ~(Attached | Disposed),
	};
public:
#if OBJECT_TRACKING
	static GHashTable *objects_alive;
	char *GetStackTrace (const char *prefix);
	char *GetStackTrace () { return GetStackTrace (""); }
	void PrintStackTrace ();
	void Track (const char *done, const char *typname);
#endif
	
	/* @GenerateCBinding,GeneratePInvoke */
	void ref ();
	
	/* @GenerateCBinding,GeneratePInvoke */
	void unref ();
	
	int GetRefCount () { return refcount; }
	int GetId () { return flags & IdMask; }
	
	//
	// Is:
	//    Similar to C#'s is: it checks if this object is of this kind or 
	//    a derived class.
	
	bool Is (Type::Kind k)
	{
		return Type::IsSubclassOf (GetObjectType (), k);
	}
	
	Type *GetType ()
	{
		return Type::Find (GetObjectType ());
	}

	/* @GenerateCBinding,GeneratePInvoke */
	virtual const char *GetTypeName ()
	{
		return Type::Find (GetObjectType ())->GetName ();
	}	
	
	/* @GenerateCBinding,GeneratePInvoke */
 	int AddHandler (const char *event_name, EventHandler handler, gpointer data, GDestroyNotify data_dtor = NULL);
	/* @GenerateCBinding,GeneratePInvoke */
 	int AddXamlHandler (const char *event_name, EventHandler handler, gpointer data, GDestroyNotify data_dtor = NULL);
	/* @GenerateCBinding,GeneratePInvoke */
 	void RemoveHandler (const char *event_name, EventHandler handler, gpointer data);

	int AddHandler (int event_id, EventHandler handler, gpointer data, GDestroyNotify data_dtor = NULL);
	int AddXamlHandler (int event_id, EventHandler handler, gpointer data, GDestroyNotify data_dtor = NULL);
	void RemoveHandler (int event_id, EventHandler handler, gpointer data);
	void RemoveHandler (int event_id, int token);
	void RemoveAllHandlers (gpointer data);
	void RemoveMatchingHandlers (int event_id, EventHandlerPredicate predicate, gpointer closure);
	
	/* @GenerateCBinding,GeneratePInvoke */
	Surface *GetSurface ();
	virtual void SetSurface (Surface *surface);
	// SetSurfaceLock/Unlock
	//  If AddTickCallSafe is called on a type, that type must override SetSurface and surround the call to its base SetSurface implementation
	//  with Lock/Unlock. Catch: none of the base implementation can cause SetSurfaceLock to be called again, it might cause a dead-lock.
	//  (This could happen if a MediaElement could contain another MediaElement, in which case DependencyObject::SetSurface would cause 
	//  the contained MediaElement's SetSurface(Lock) to be called).
	bool SetSurfaceLock ();
	void SetSurfaceUnlock ();
	
	// AddTickCall*: 
	//  Queues a delegate which will be called on the main thread.
	//  The delegate's parameter will be the 'this' pointer.
	//  Only AddTickCallSafe is safe to call on threads other than the main thread,
	//  and only if the type on which it is called overrides SetSurface and surrounds
	//  the call to the base type's SetSurface with SetSurfaceLock/Unlock.
	void AddTickCall (TickCallHandler handler);
	void AddTickCallSafe (TickCallHandler handler);

	/* @GenerateCBinding,GeneratePInvoke */
	void SetObjectType (Type::Kind value) { object_type = value; }

	/* @GenerateCBinding,GeneratePInvoke */
	virtual Type::Kind GetObjectType () { return object_type; }

	const static int DestroyedEvent;
	
	void unref_delayed ();
	
	EmitContext *StartEmit (int event_id);
	bool DoEmit (int event_id, EmitContext *ctx, EventArgs *calldata = NULL, bool only_unemitted = false);
	void FinishEmit (int event_id, EmitContext *ctx);
	static gboolean EmitCallback (gpointer d);
	
	virtual void Dispose ();
	
	bool IsAttached ();
	void SetIsAttached (bool value);
	bool IsDisposed ();
	bool IsMultiThreadedSafe () { return (flags & MultiThreadedSafe) != 0; }
	
	Deployment *GetDeployment ();
	void SetCurrentDeployment (bool domain = true);

#if SANITY
	Deployment *GetUnsafeDeployment () { return deployment; } // a public deployment getter for sanity checking without the warnings in GetDeployment.
#endif

	/* @GenerateCBinding,GeneratePInvoke */
	void AddToggleRefNotifier (ToggleNotifyHandler tr);
	/* @GenerateCBinding,GeneratePInvoke */
	void RemoveToggleRefNotifier ();

protected:
	virtual ~EventObject ();
	EventObject ();
	EventObject (Type::Kind type);
	EventObject (Type::Kind type, bool multi_threaded_safe);
	EventObject (Deployment *deployment);
	EventObject (Deployment *deployment, Type::Kind type);
	
	// To enable scenarios like Emit ("Event", new EventArgs ())
	// Emit will call unref on the calldata.
	bool Emit (char *event_name, EventArgs *calldata = NULL, bool only_unemitted = false);
	bool Emit (int event_id, EventArgs *calldata = NULL, bool only_unemitted = false);

private:
	void AddTickCallInternal (TickCallHandler handler);
	void Initialize (Deployment *deployment, Type::Kind type);

	EventLists *events;
	Surface *surface; // TODO: Remove this (along with SetSurface)
	Deployment *deployment;
	gint32 refcount;
	gint32 flags; // Don't define as Flags, we need to keep this reliably at 32 bits.

	Type::Kind object_type;
	ToggleNotifyListener *toggleNotifyListener;
};


/* @Namespace=System.Windows */
class DependencyObject : public EventObject {
public:
 	/* @GenerateCBinding,GeneratePInvoke */
	DependencyObject ();
	virtual void Dispose ();

	void Freeze ();
	
	DependencyProperty **GetProperties (bool only_changed);
	
	GHashTable *GetLocalValues () { return local_values; }

	// Gets the content property from this object's type, and
	// returns the value of that dependency property.
	//
	DependencyObject *GetContent ();
	DependencyProperty *GetDependencyProperty (const char *name);
	
	bool SetValue (DependencyProperty *property, Value *value);
	bool SetValue (DependencyProperty *property, Value value);
	
	bool SetValue (int property, Value *value);
	bool SetValue (int property, Value value);

	/* @GenerateCBinding,GeneratePInvoke */
	bool SetValueWithError (DependencyProperty *property, Value *value, MoonError *error);
	bool SetValueWithError (DependencyProperty *property, Value value, MoonError *error);

	/* @GenerateCBinding,GeneratePInvoke,Version=2.0 */
	Value *GetValueWithError (Type::Kind whatami, DependencyProperty *property, MoonError *error);
	virtual Value *GetValue (DependencyProperty *property);
	Value *GetValue (int id);

	void ProviderValueChanged (PropertyPrecedence providerPrecedence, DependencyProperty *property, Value *old_value, Value *new_value, bool notify_listeners, MoonError *error);
	Value *GetValue (DependencyProperty *property, PropertyPrecedence startingAtPrecedence);
	Value *GetValueSkippingPrecedence (DependencyProperty *property, PropertyPrecedence toSkip);
	
	/* @GenerateCBinding,GeneratePInvoke,Version=2.0 */
	Value *ReadLocalValueWithError (DependencyProperty *property, MoonError *error);
	virtual Value *ReadLocalValue (DependencyProperty *property);
	virtual Value *ReadLocalValue (int id);
	
	/* @GenerateCBinding,GeneratePInvoke,Version=2.0 */
	Value *GetValueNoDefaultWithError (DependencyProperty *property, MoonError *error);
	Value *GetValueNoDefault (DependencyProperty *property);
	Value *GetValueNoDefault (int id);
	
	
	/* @GenerateCBinding,GeneratePInvoke,Version=2.0 */
	virtual void ClearValue (DependencyProperty *property, bool notify_listeners, MoonError *error);
	void ClearValue (int id, bool notify_listeners, MoonError *error);
	void ClearValue (DependencyProperty *property, bool notify_listeners = true /*, error = NULL */);
	void ClearValue (int id, bool notify_listeners = true);
	bool HasProperty (const char *name, bool inherits);
	bool HasProperty (Type::Kind whatami, DependencyProperty *property, bool inherits);

	DependencyObject *FindName (const char *name);
	/* @GenerateCBinding,GeneratePInvoke */
	DependencyObject *FindName (const char *name, Type::Kind *element_kind);
	NameScope *FindNameScope ();

	/* @GenerateCBinding,GeneratePInvoke */
	const char *GetName ();
	/* @GenerateCBinding,GeneratePInvoke */
	void SetName (const char *name);

	bool SetName (const char *name, NameScope *scope);

	virtual void SetSurface (Surface *surface);

	/* @GenerateCBinding,GeneratePInvoke */
	void SetParent (DependencyObject *parent, MoonError *error);
	DependencyObject* GetParent () { return parent; }

	virtual bool PermitsMultipleParents () { return true; }

	virtual void OnPropertyChanged (PropertyChangedEventArgs *args, MoonError *error);
	
	// See the comment below about AddPropertyChangeListener for
	// the meaning of the @prop arg in this method.  it's not what
	// you might think it is.
	virtual void OnSubPropertyChanged (DependencyProperty *prop, DependencyObject *obj, PropertyChangedEventArgs *subobj_args) { }
	
	//
	// OnCollectionChanged:
	//
	// This method is invoked when a change has happened in the @col
	// collection, the kind of change is described in @type (change start,
	// change end, adding, removing, or altering an existing item).
	//
	virtual void OnCollectionChanged (Collection *col, CollectionChangedEventArgs *args) { }
	
	//
	// OnCollectionItemChanged:
	//
	// This method is invoked when an item in the collection has had a property changed.
	//
	virtual void OnCollectionItemChanged (Collection *col, DependencyObject *obj, PropertyChangedEventArgs *args) { }
	
	// These methods get called on DependencyObjects when they are
	// set/unset as property values on another DependencyObject.
	virtual void AddTarget (DependencyObject *obj) { }
	virtual void RemoveTarget (DependencyObject *obj) { }
	
	// These two methods are a little confusing.  @child_property
	// is *not* the property you're interested in receiving change
	// notifications on.  Listeners are always notified of all
	// changes.  What @child_property is used for is so that the
	// listener can look at it and know which of its *own*
	// properties is reporting the change.  So, if a object wants
	// to listen for changes on its BackgroundProperty, it would
	// essentially do:
	//
	// background = GetValue(BackgroundProperty)->AsDependencyObject();
	// background->AddPropertyChangeListener (this, BackgroundProperty);
	//
	// then in its OnSubPropertyChanged method, it could check prop to
	// see if it's == BackgroundProperty and act accordingly.  The
	// child's changed property is contained in the @subobj_args
	// argument of OnSubPropertyChanged.

	void AddPropertyChangeListener (DependencyObject *listener, DependencyProperty *child_property = NULL);
	void RemovePropertyChangeListener (DependencyObject *listener, DependencyProperty *child_property = NULL);

	// *These* two methods do what you'd expect.  You provide this
	// dependencyobject with a callback and a closure to be
	// invoked when the given property changes.

	/* @GenerateCBinding,GeneratePInvoke */
	void AddPropertyChangeHandler (DependencyProperty *property, PropertyChangeHandler cb, gpointer closure);
	/* @GenerateCBinding,GeneratePInvoke */
	void RemovePropertyChangeHandler (DependencyProperty *property, PropertyChangeHandler cb);

	virtual void UnregisterAllNamesRootedAt (NameScope *from_ns);
	virtual void RegisterAllNamesRootedAt (NameScope *to_ns, MoonError *error);

	/* @PropertyType=string,GenerateAccessors,ManagedDeclaringType=FrameworkElement,Validator=NameValidator,DefaultValue=\"\" */
	const static int NameProperty;
	
	// parser hook.  objects that are parsed using XamlReader.Load
	// behave differently than those parsed using LoadComponent in
	// terms of their name registration behavior.
	void SetIsHydratedFromXaml (bool flag) { is_hydrated = flag; }
	bool IsHydratedFromXaml () { return is_hydrated; }

protected:
	virtual ~DependencyObject ();
	DependencyObject (Deployment *deployment, Type::Kind object_type = Type::DEPENDENCY_OBJECT);
	
	//
	// Returns true if a value is valid.  If the value is invalid return false.
	// If error is non NULL and the value is not valid, error will be given an error code and error message that should be
	// propogated to OnError
	//
	bool IsValueValid (DependencyProperty *property, Value *value, MoonError *error);
	
	virtual bool SetValueWithErrorImpl (DependencyProperty *property, Value *value, MoonError *error);
	
	void NotifyListenersOfPropertyChange (PropertyChangedEventArgs *args, MoonError *error);
	void NotifyListenersOfPropertyChange (DependencyProperty *property, MoonError *error);
	void NotifyListenersOfPropertyChange (int id, MoonError *error);
	
	void RemoveAllListeners ();

	PropertyValueProvider **providers;

private:
	void RemoveListener (gpointer listener, DependencyProperty *child_property);
	void Initialize ();

	static void collection_changed (EventObject *sender, EventArgs *args, gpointer closure);
	static void collection_item_changed (EventObject *sender, EventArgs *args, gpointer closure);

	static gboolean dispose_value (gpointer key, gpointer value, gpointer data);

	GHashTable        *local_values;
	GSList            *listener_list;
	DependencyObject  *parent;

	bool is_frozen;
	bool is_hydrated;
};

#endif /* __MONO_DEPOBJECT_H__ */
