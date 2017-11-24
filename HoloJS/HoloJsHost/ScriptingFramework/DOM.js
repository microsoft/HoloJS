// This file was automatically generated; DO NOT EDIT.
/************************************************************************
Copyright (c) 2011 The Mozilla Foundation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************/
(function(global) {
/************************************************************************
 * src/loose.js
 ************************************************************************/

//@line 1 "src/loose.js"
// This file holds any dom.js code that won't work in strict mode

//
// This is a simple class used in src/impl/HTMLEvent.js and elsewhere for
// converting an HTML event handler content attribute into a function with
// a special scope chain.  It uses with, so it can't be in strict mode.
//
// This is a class with a build() method instead of a simple function because
// inside of with statements it is hard to refer to the function arguments.
// By using this class-based design, I can refer to them as properties of this.
//
// The body argument is a string of js code that becomes the body of the
// event handler function.
//
// The document, form, and element arguments to the constructor must be
// wrapper objects, not impl objects.  Note that we can't call wrap from
// this file, so they must be passed in wrapped.
//

function EventHandlerBuilder(body, document, form, element) {
    this.body = body;
    this.document = document;
    this.form = form;
    this.element = element;
}

EventHandlerBuilder.prototype.build = function build() {
    with(this.document)
        with(this.form)
            with(this.element)
                return eval("(function(event){" + this.body + "})");
};

// Define this here so it is not in strict mode
// Because strict mode eval can't define variables.
function evalScript(s) {
    var geval = eval; // Do an indirect eval to get global context
    s = 'try{' +
        s +
        '}catch(e){var msg="exception while evaling script:\\n\\t" + e.message + "\\n" + e.stack; console.log(msg); throw Error(msg);}';

    geval(s);
}(function(global) {
"use strict";



/************************************************************************
 *  src/snapshot.js
 ************************************************************************/

//@line 1 "src/snapshot.js"
/*
 * We want to be sure that we only use the built-in versions of standard
 * functions and methods like Object.create and Array.prototype.pop.
 * So here we make snapshots of all the system objects, and then define
 * utility functions that use them.
 *
 * It is an error if any of the built-in methods are used anywhere else
 * in dom.js after this initial snapshot.
 *
 * The utilities defined here use a functional syntax rather than the
 * OO syntax of the JS builtins.  Instead of a.map(f), we call map(a, f)
 * for example.
 *
 * See ../test/monkey.js for code that patches all the built-in
 * functions and methods to test whether we avoid using them.
 */

function shallow_frozen_copy(o) {
    var r = {};
    Object.getOwnPropertyNames(o).forEach(function(n) {
        Object.defineProperty(r, n, Object.getOwnPropertyDescriptor(o, n));
    });
    return Object.freeze(r);
}

const undefined = void 0,

    // Copy the original state of constructor functions
    // This is not a complete list. I've left out error types I'm unlikely
    // to ever throw.

    Array = global.Array,
    Boolean = global.Boolean,
    Date = global.Date,
    Error = global.Error,
    Function = global.Function,
    Number = global.Number,
    Object = global.Object,
    RangeError = global.RangeError,
    RegExp = global.RegExp,
    String = global.String,
    TypeError = global.TypeError,
    WeakMap = global.WeakMap;


// callbind parameterizes the binding of `this`
// [].map(callback) -> map([], callback)
const callbind = Function.prototype.call.bind.bind(Function.prototype.call);


// String and array generics are not defined in Node, so define them now
// if needed
if (!String.indexOf) {
    Object.getOwnPropertyNames(String.prototype).forEach(function(m) {
        if (typeof String.prototype[m] !== "function") return;
        if (m === "length" || m === "constructor") return;
        String[m] = callbind(String.prototype[m]);
    });
}

if (!Array.forEach) {
    Object.getOwnPropertyNames(Array.prototype).forEach(function(m) {
        if (typeof Array.prototype[m] !== "function") return;
        if (m === "length" || m === "constructor") return;
        Array[m] = callbind(Array.prototype[m]);
    });
}


const
    // Some global functions.
    // Note that in strict mode we're not allowed to create new identifiers
    // named eval.  But if we give eval any other name then it does a
    // global eval instead of a local eval. I shouldn't ever need to use it,
    // so just omit it here.
    parseInt = global.parseInt,
    parseFloat = global.parseFloat,
    isNaN = global.isNaN,
    isFinite = global.isFinite,

    // Snapshot objects that hold a lot of static methods
    // We also want to make a snapshot of the static methods of Object, Array,
    // and String. (Firefox defines useful "Array generics" and "String
    // generics" that are quite helpful to us).  Since we've already bound
    // the names Object, Array, and String, we use O, A, and S as shorthand
    // notation for these frequently-accessed sets of methods.
    JSON = shallow_frozen_copy(global.JSON),
    Math = shallow_frozen_copy(global.Math),
    Proxy = shallow_frozen_copy(global.Proxy),
    O = shallow_frozen_copy(Object),
    A = shallow_frozen_copy(Array),
    S = shallow_frozen_copy(String),

    // Copy some individual static methods from types that don't
    // define very many.
    now = Date.now,

    // Note that it is never safe to invoke a method of a built-in
    // object except in code that is going to run right now. The
    // functions defined below provide a safe alternative, but mandate
    // a functional style of programming rather than an OO style.

    // Functions
    // call(f, o, args...)
    call = callbind(Function.prototype.call),
    // apply(f, o, [args])
    apply = callbind(Function.prototype.apply),
    // bind(f, o)
    bind = callbind(Function.prototype.bind),

    // WeakMap functions
    wmget = callbind(WeakMap.prototype.get),
    wmset = callbind(WeakMap.prototype.set),

    // Object functions
    hasOwnProperty = callbind(Object.prototype.hasOwnProperty),

    // Array functions are all defined as generics like A.pop, but its
    // convenient to give the most commonly-used ones unqualified
    // names.  The less-commonly used functions (and those that have
    // name collisions like indexOf, lastIndexOf and slice) can be
    // accessed on the A or S objects.
    concat = A.concat || callbind(Array.prototype.concat),
    every = A.every || callbind(Array.prototype.every),
    // Note lowercase e
    foreach = A.forEach || callbind(Array.prototype.forEach),
    isArray = A.isArray || callbind(Array.prototype.isArray),
    join = A.join || callbind(Array.prototype.join),
    map = A.map || callbind(Array.prototype.map),
    push = A.push || callbind(Array.prototype.push),
    pop = A.pop || callbind(Array.prototype.pop),
    unshift = A.unshift || callbind(Array.prototype.unshift),
    reduce = A.reduce || callbind(Array.prototype.reduce),
    sort = A.sort || callbind(Array.prototype.sort),
    filter = A.filter || callbind(Array.prototype.filter),
    splice = A.splice || callbind(Array.prototype.splice),

    // Ditto for the String generic functions
    fromCharCode = S.fromCharCode || callbind(String.prototype.fromCharCode),
    match = S.match || callbind(String.prototype.match),
    replace = S.replace || callbind(String.prototype.replace),
    search = S.search || callbind(String.prototype.search),
    split = S.split || callbind(String.prototype.split),
    substring = S.substring || callbind(String.prototype.substring),
    toLowerCase = S.toLowerCase || callbind(String.prototype.toLowerCase),
    toUpperCase = S.toUpperCase || callbind(String.prototype.toUpperCase),
    trim = S.trim || callbind(String.prototype.trim),

    // One more array-related function
    pushAll = Function.prototype.apply.bind(Array.prototype.push),

    // RegExp functions, too
    exec = callbind(RegExp.prototype.exec),
    test = callbind(RegExp.prototype.test)

    ;

// These are all unique and have their uses, particularly for formatting.
// Also, only when accessing directly from primitive wrapper (string/number) is
// the native version assured to be used.
const toString = Object.freeze({
    // `[].join(',')`
    Array: callbind(Array.prototype.toString),
    // 'true' or 'false'
    Boolean: callbind(Boolean.prototype.toString),
    // "Sat Dec 10 2011 23:40:56 GMT-0500 (US Eastern Standard Time)"
    Date: callbind(Date.prototype.toString),
    // Works generically, `e.name + ' ' + e.message`
    Error: callbind(Error.prototype.toString),
    // unmodified source in V8, normalized in spidermonkey
    Function: callbind(Function.prototype.toString),
    // Works generically, '[object InternalClass]'
    Object: callbind(Object.prototype.toString),
    // input must be number, has optional radix parameter
    Number: callbind(Number.prototype.toString),
    // RegExp('^\s+', 'g').toString() -> '/\s+/g'
    RegExp: callbind(RegExp.prototype.toString),
    // input must be string
    String: callbind(String.prototype.toString)
});


/************************************************************************
 *  src/globals.js
 ************************************************************************/

//@line 1 "src/globals.js"
// These constants and variables aren't really globals.  They're all within
// the closure added by the Makefile, so they don't affect the global
// environment.  But they are visible everywhere within dom.js

// Namespaces
const HTML_NAMESPACE = "http://www.w3.org/1999/xhtml";
const XML_NAMESPACE = "http://www.w3.org/XML/1998/namespace";
const XMLNS_NAMESPACE = "http://www.w3.org/2000/xmlns/";
const MATHML_NAMESPACE = "http://www.w3.org/1998/Math/MathML";
const SVG_NAMESPACE = "http://www.w3.org/2000/svg";
const XLINK_NAMESPACE = "http://www.w3.org/1999/xlink";

// Anything I want to define lazily using defineLazyProperty above has
// to be a property of something; it can't just be a local variable.
// So these objects are holders for lazy properties.
const impl = {}; // implementation construtors defined here
const idl = {};  // interface constructors defined here


// Sometimes we need to know the document of the current script.
// So this global is the <script> element that's running if there is one.
var currentlyExecutingScript = null;


/************************************************************************
 *  src/utils.js
 ************************************************************************/

//@line 1 "src/utils.js"
// Utility functions and other globals used throughout dom.js

function assert(expr, msg) {
    if (!expr) {
        throw new Error("Assertion failed: " + (msg || "") + "\n" + new Error().stack);
    }
}

// For stuff that I haven't implemented yet
function nyi() {
    var e = new Error();
    var where = split(e.stack, "\n")[1];
    e.name = "NotYetImplemented";
    e.message = where;
    throw e;
}

// Called by deprecated functions, etc.
function warn(msg) {
    if (global.console) console.warn(msg);
    else if (global.print) {
        print("WARNING: " + msg);
    }
}

// Currently this is only used to report errors when running scripts
function error(msg) {
    if (global.console && global.console.error)
        console.error(msg);
    else if (global.print) {
        print("ERROR: " + msg);
    }
}

// Utility functions that return property descriptors
function constant(v) { return { value: v }; }
function attribute(get, set) {
    if (set)
        return { get: get, set: set};
    else
        return { get: get };
}

// some functions that do very simple stuff
// Note that their names begin with f.
// This is good for things like attribute(fnull,fnoop)
function fnull() { return null; }
function ftrue() { return true; }
function ffalse() { return false; }
function fnoop() { /* do nothing */ }

const readonlyPropDesc = {writable:false,enumerable:true,configurable: true};
const hiddenPropDesc = {writable: true,enumerable: false,configurable: true};
const constantPropDesc = {writable: false,enumerable: true,configurable: false};
const hiddenConstantPropDesc = {
    writable: false, enumerable: false, configurable: false
};

// Set o.p to v, but make the property read-only
function defineReadonlyProp(o,p,v) {
    readonlyPropDesc.value = v;
    O.defineProperty(o, p, readonlyPropDesc);
}

// Set o.p to v, but make the property non-enumerable
function defineHiddenProp(o,p,v) {
    hiddenPropDesc.value = v;
    O.defineProperty(o, p, hiddenPropDesc);
}

// Set o.p to v, and make it constant
function defineConstantProp(o,p,v) {
    constantPropDesc.value = v;
    O.defineProperty(o, p, constantPropDesc);
}

// Set o.p to v, and make it constant and non-enumerable
function defineHiddenConstantProp(o,p,v) {
    hiddenConstantPropDesc.value = v;
    O.defineProperty(o, p, hiddenConstantPropDesc);
}

//
// Define a property p of the object o whose value is the return value of f().
// But don't invoke f() until the property is actually used for the first time.
// The property will be writeable, enumerable and configurable.
// If the property is made read-only before it is used, then it will throw
// an exception when used.
// Based on Andreas's AddResolveHook function.
//
function defineLazyProperty(o, p, f, hidden, readonly) {
    O.defineProperty(o, p, {
        get: function() {            // When the property is first retrieved
            var realval = f();       // compute its actual value
            O.defineProperty(o, p, { // Store that value
                value: realval,
                writable: !readonly,
                enumerable: !hidden,
                configurable: true
            });
            return realval;          // And return the computed value
        },
        set: readonly ? undefined : function(newval) {
            // If the property is writable and is set before being read,
            // just replace the value and f() will never be invoked

            // Remove the line below when this bug is fixed:
            // https://bugzilla.mozilla.org/show_bug.cgi?id=703157
            delete o[p];

            O.defineProperty(o, p, {
                value: newval,
                writable: !readonly,
                enumerable: !hidden,
                configurable: true
            });
        },
        enumerable: !hidden,
        configurable: true
    });
}


// Compare two nodes based on their document order. This function is intended
// to be passed to sort(). Assumes that the array being sorted does not
// contain duplicates.  And that all nodes are connected and comparable.
// Clever code by ppk via jeresig.
function documentOrder(n,m) {
    return 3 - (n.compareDocumentPosition(m) & 6);
}

// Like String.trim(), but uses HTML's definition of whitespace
// instead of using Unicode's definition of whitespace.
function htmlTrim(s) {
    return s.replace(/^[ \t\n\r\f]+|[ \t\n\r\f]+$/g, "");
}


/************************************************************************
 *  src/wrapmap.js
 ************************************************************************/

//@line 1 "src/wrapmap.js"
// dom.js uses two kinds of tree node objects.  nodes (with a
// lowercase n) are the internal data structures that hold the actual
// document data. They are implemented by the files in impl/* Nodes
// (with a capital N) are the public objects that implement DOM
// interfaces and do not have any properties other than the accessor
// properties and methods defined by the DOM.  They are implemented by
// the files in idl/*
//
// Every Node must have a node to hold its actual data.
// But nodes can exist without any corresponding Node: Nodes are created
// as needed, when scripts use the DOM API to inspect the document tree.
//
// Since Node objects can't have properties, the mapping from Node to node
// is done with a WeakMap.  The mapping from node to Node is simpler:
// if a Node exists for the node, it is simply set on a property of the node.
//
// The methods in this file manage the mapping between nodes and Nodes
//

var idlToImplMap = new WeakMap(), lastkey = {}, lastvalue = undefined;

// Return the implementation object for the DOM Node n
// This method will throw a TypeError if n is
// null, undefined, a primitive, or an object with no mapping.
// This provides basic error checking for methods like Node.appendChild().
// XXX: We used to throw NOT_FOUND_ERR here, but ms2ger's tests
// expect TypeError
function unwrap(n) {
    // Simple optimization
    // If I ever remove or alter mappings, then this won't be valid anymore.
    if (n === lastkey)
        return lastvalue;

    try {
        var impl = wmget(idlToImplMap, n);

        // This happens if someone passes a bogus object to
        // appendChild, for example.
        if (!impl) NotFoundError();

        lastkey = n;
        lastvalue = impl;
        return impl;
    }
    catch(e) {
        // If n was null or not an object the WeakMap will raise a TypeError
        // TypeError might be the best thing to propagate, but there is also
        // some precendent for raising a DOMException with code
        // NOT_FOUND_ERR;
        throw TypeError();
    }
}

function unwrapOrNull(n) {
    return n
        ? unwrap(n)
        : null;
}

// Return the interface object (a DOM node) for the implementation object n,
// creating it if necessary. Implementation objects define the type
// of wrapper they require by defining an _idlName property. Most classes
// do this on their prototype.  For childNodes and attributes arrays,
// we have to define _idlName directly on the array objects, however.
function wrap(n) {
    if (n === null) return null;

    if (n === undefined)
        throw new Error("Can't wrap undefined property");

    // If n doesn't have a wrapper already, create one.
    if (!n._idl) {
        var typename = n._idlName;
        if (!typename)
            throw Error("Implementation object does not define _idlName");
        var type = idl[typename];
        if (!type)
            throw Error("Unknown idl type " + typename);

        n._idl = type.factory(n);       // Create the wrapper
        wmset(idlToImplMap, n._idl, n); // Remember it for unwrap()
    }

    return n._idl;
}




/************************************************************************
 *  src/xmlnames.js
 ************************************************************************/

//@line 1 "src/xmlnames.js"
// This grammar is from the XML and XML Namespace specs. It specifies whether
// a string (such as an element or attribute name) is a valid Name or QName.
//
// Name            ::=          NameStartChar (NameChar)*
// NameStartChar   ::=          ":" | [A-Z] | "_" | [a-z] |
//                              [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] |
//                              [#x370-#x37D] | [#x37F-#x1FFF] |
//                              [#x200C-#x200D] | [#x2070-#x218F] |
//                              [#x2C00-#x2FEF] | [#x3001-#xD7FF] |
//                              [#xF900-#xFDCF] | [#xFDF0-#xFFFD] |
//                              [#x10000-#xEFFFF]
//
// NameChar        ::=          NameStartChar | "-" | "." | [0-9] |
//                                 #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
//
// QName           ::=          PrefixedName| UnprefixedName
// PrefixedName    ::=          Prefix ':' LocalPart
// UnprefixedName  ::=          LocalPart
// Prefix          ::=          NCName
// LocalPart       ::=          NCName
// NCName          ::=          Name - (Char* ':' Char*)
//                              # An XML Name, minus the ":"
//
const xml = (function() {

    // Most names will be ASCII only. Try matching against simple regexps first
    var simplename = /^[_:A-Za-z][-.:\w]+$/;
    var simpleqname = /^([_A-Za-z][-.\w]+|[_A-Za-z][-.\w]+:[_A-Za-z][-.\w]+)$/

    // If the regular expressions above fail, try more complex ones that work
    // for any identifiers using codepoints from the Unicode BMP
    var ncnamestartchars = "_A-Za-z\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u02ff\u0370-\u037D\u037F-\u1FFF\u200C-\u200D\u2070-\u218F\u2C00-\u2FEF\u3001-\uD7FF\uF900-\uFDCF\uFDF0-\uFFFD";
    var ncnamechars = "-._A-Za-z0-9\u00B7\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u02ff\u0300-\u037D\u037F-\u1FFF\u200C\u200D\u203f\u2040\u2070-\u218F\u2C00-\u2FEF\u3001-\uD7FF\uF900-\uFDCF\uFDF0-\uFFFD";

    var ncname = "[" + ncnamestartchars + "][" + ncnamechars + "]*";
    var namestartchars = ncnamestartchars + ":";
    var namechars = ncnamechars + ":";
    var name = new RegExp("^[" + namestartchars + "]" +
                          "[" + namechars + "]*$");
    var qname = new RegExp("^(" + ncname + "|" +
                           ncname + ":" + ncname + ")$");

    // XML says that these characters are also legal:
    // [#x10000-#xEFFFF].  So if the patterns above fail, and the
    // target string includes surrogates, then try the following
    // patterns that allow surrogates and then run an extra validation
    // step to make sure that the surrogates are in valid pairs and in
    // the right range.  Note that since the characters \uf0000 to \u1f0000
    // are not allowed, it means that the high surrogate can only go up to
    // \uDB7f instead of \uDBFF.
    var hassurrogates = /[\uD800-\uDB7F\uDC00-\uDFFF]/;
    var surrogatechars = /[\uD800-\uDB7F\uDC00-\uDFFF]/g;
    var surrogatepairs = /[\uD800-\uDB7F][\uDC00-\uDFFF]/g;

    // Modify the variables above to allow surrogates
    ncnamestartchars += "\uD800-\uDB7F\uDC00-\uDFFF";
    ncnamechars += "\uD800-\uDB7F\uDC00-\uDFFF";
    ncname = "[" + ncnamestartchars + "][" + ncnamechars + "]*";
    namestartchars = ncnamestartchars + ":";
    namechars = ncnamechars + ":";

    // Build another set of regexps that include surrogates
    var surrogatename = new RegExp("^[" + namestartchars + "]" +
                                   "[" + namechars + "]*$");
    var surrogateqname = new RegExp("^(" + ncname + "|" +
                                    ncname + ":" + ncname + ")$");

    function isValidName(s) {
        if (test(simplename, s)) return true;  // Plain ASCII
        if (test(name, s)) return true;        // Unicode BMP

        // Maybe the tests above failed because s includes surrogate pairs
        // Most likely, though, they failed for some more basic syntax problem
        if (!test(hassurrogates, s)) return false;

        // Is the string a valid name if we allow surrogates?
        if (!test(surrogatename, s)) return false;

        // Finally, are the surrogates all correctly paired up?
        var chars = match(s, surrogatechars), pairs = match(s, surrogatepairs);
        return pairs != null && 2*pairs.length === chars.length;
    }


    function isValidQName(s) {
        if (test(simpleqname, s)) return true;  // Plain ASCII
        if (test(qname, s)) return true;        // Unicode BMP

        if (!test(hassurrogates, s)) return false;
        if (!test(surrogateqname, s)) return false;
        var chars = match(s, surrogatechars), pairs = match(s, surrogatepairs);
        return pairs != null && 2*pairs.length === chars.length;
    }

    return {isValidName: isValidName, isValidQName: isValidQName};
}());



/************************************************************************
 *  src/idl.js
 ************************************************************************/

//@line 1 "src/idl.js"
// This file defines functions for satisfying the requirements of WebIDL
// See also ../tools/idl2domjs

// WebIDL requires value conversions in various places.

// Convert x to an unsigned long and return it
// WebIDL currently says to use ES ToUint32() unless there is a [Clamp]
// attribute on the operation.  We can invoke the ToUint32 operation
// with the >>> operator.
//
function toULong(x) {
    return x >>> 0;  // The >>> operator does ToUint32
}

function toLong(x) {
    return x & 0xFFFFFFFF; // This should do ToInt32
}

function toUShort(x) {
    return (x >>> 0) & 0xFFFF;  // Convert to uint32, and then truncate.
}

// Convert the value x to a number, and raise an exception if
// it is NaN or infinite. This is not actually part of WebIDL:
// HTML mandates this check "except where otherwise specified".
// I'll probably want to change the idl type of attributes or
// arguments for which NaN and infinite values are allowed, if
// there are any
function toDouble(x) {
    var v = Number(x)
    if (!isFinite(v)) NotSupportedError(x + " is not a finite float.");
    return v;
}

function undef2null(x) { return x === undefined ? null : x; }

// Convert x to a string as with the String() conversion function.
// But if x is null, return the empty string insead of "null".
// If a WebIDL method argument is just DOMString, convert with String()
// But if it is [TreatNullAs=EmptyString] DOMString then use this function.
function StringOrEmpty(x) {
    return (x === null) ? "" : String(x);
}

function StringOrNull(x) {
    return (x === null) ? null : String(x);
}

function OptionalStringOrNull(x) {
    return x === null || x === undefined ? null : String(x);
}

function OptionaltoLong(x){
    return x === undefined ? undefined : toLong(x);
}

function OptionalBoolean(x) {
    return (x === undefined) ? undefined : Boolean(x);
}

function OptionalObject(x) {
    return (x === undefined) ? undefined : Object(x);
}

function toCallback(x) {
    var t = typeof x;
    if (t === "function" || t === "object") return x;
    else throw TypeError("Expected callback; got: " + x);
}

function toCallbackOrNull(x) {
    return (x === null) ? null : toCallback(x);
}

// This constructor takes a single object as its argument and looks for
// the following properties of that object:
//
//    name         // The name of the interface
//    superclass   // The superclass constructor
//    proxyHandler // The proxy handler constructor, if one is needed
//    constants    // constants defined by the interface
//    members      // interface attributes and methods
//    constructor  // optional public constructor.
//
// It returns a new object with the following properties:
//   publicInterface // The public interface to be placed in the global scope
//                   // The input constructor or a synthetic no-op one.
//   prototype       // The prototype object for the interface
//                   // Also available as publicInterface.prototype
//   factory         // A factory function for creating an instance
//
function IDLInterface(o) {
    var name = o.name || "";
    var superclass = o.superclass;
    var proxyFactory = o.proxyFactory;
    var constants = o.constants || {};
    var members = o.members || {};
    var prototype, interfaceObject;

    // Set up the prototype object
    prototype = superclass ? O.create(superclass.prototype) : {};

    if (hasOwnProperty(o, "constructor")) {
        interfaceObject = o.constructor;
    }
    else {
        // The interface object is supposed to work with instanceof, but is
        // not supposed to be callable.  We can't conform to both requirements
        // so we make the interface object a function that throws when called.
        interfaceObject = function() {
            throw new TypeError(name + " is not (supposed to be) a function");
        };
    }

    // WebIDL says that the interface object has this prototype property
    interfaceObject.prototype = prototype;
    // Make it read-only
    O.defineProperty(interfaceObject, "prototype", { writable: false });
    // XXX: the line below works in spidermonkey, but not in node.
    // probably related the fact that prototype already exists and
    // is writable but non-configurable?
    //  defineHiddenConstantProp(interfaceObject, "prototype", prototype);

    // WebIDL also says that the prototype points back to the interface object
    // instead of the real constructor.
    defineHiddenProp(prototype, "constructor", interfaceObject);

    // Constants must be defined on both the prototype and interface objects
    // And they must read-only and non-configurable
    for(var c in constants) {
        var value = constants[c];
        defineConstantProp(prototype, c, value);
        defineConstantProp(interfaceObject, c, value);
    }

    // Now copy attributes and methods onto the prototype object.
    // Members should just be an ordinary object.  Attributes should be
    // defined with getters and setters. Methods should be regular properties.
    // This will mean that the members will all be enumerable, configurable
    // and writable (unless there is no setter) as they are supposed to be.
    for(var m in members) {
        // Get the property descriptor of the member
        var desc = O.getOwnPropertyDescriptor(members, m);

        // Now copy the property to the prototype object
        O.defineProperty(prototype, m, desc);
    }

    // If the interface does not already define a toString method, add one.
    // This will help to make debugging easier.
    //
    // XXX: I'm not sure if this is legal according to WebIDL and DOM Core.
    // XXX Maybe I could move it down to an object on the prototype chain
    // above Object.prototype.  But then I'd need some way to determine
    // the type name.  Maybe the name of the public "constructor" function?
    // But then I'd have to create that function with eval, I think.
    if (!hasOwnProperty(members, "toString")) {
        prototype.toString = function() { return "[object " + name + "]"; };
    }

    // Now set up the fields of this object
    this.prototype = prototype;
    this.publicInterface = interfaceObject;
    this.factory = proxyFactory
        ? proxyFactory
        : O.create.bind(Object, prototype, {});
}




/************************************************************************
 *  src/domcore.js
 ************************************************************************/

//@line 1 "src/domcore.js"
//
// DO NOT EDIT.
// This file was generated by idl2domjs from src/idl/domcore.idl
//


//
// Interface Event
//

// Constants defined by Event
const CAPTURING_PHASE = 1;
const AT_TARGET = 2;
const BUBBLING_PHASE = 3;

defineLazyProperty(global, "Event", function() {
    return idl.Event.publicInterface;
}, true);

defineLazyProperty(idl, "Event", function() {
    return new IDLInterface({
        name: "Event",
        constructor: function Event(
                                type,
                                eventInitDict)
        {
            return wrap(new impl.Event(
                               String(type),
                               OptionalEventInit(eventInitDict)));
        },
        constants: {
            CAPTURING_PHASE: CAPTURING_PHASE,
            AT_TARGET: AT_TARGET,
            BUBBLING_PHASE: BUBBLING_PHASE,
        },
        members: {
            get type() {
                return unwrap(this).type;
            },

            get target() {
                return wrap(unwrap(this).target);
            },

            get currentTarget() {
                return wrap(unwrap(this).currentTarget);
            },

            get eventPhase() {
                return unwrap(this).eventPhase;
            },

            stopPropagation: function stopPropagation() {
                unwrap(this).stopPropagation();
            },

            stopImmediatePropagation: function stopImmediatePropagation() {
                unwrap(this).stopImmediatePropagation();
            },

            get bubbles() {
                return unwrap(this).bubbles;
            },

            get cancelable() {
                return unwrap(this).cancelable;
            },

            preventDefault: function preventDefault() {
                unwrap(this).preventDefault();
            },

            get defaultPrevented() {
                return unwrap(this).defaultPrevented;
            },

            get isTrusted() {
                return unwrap(this).isTrusted;
            },

            get timeStamp() {
                return unwrap(this).timeStamp;
            },

            initEvent: function initEvent(
                                    type,
                                    bubbles,
                                    cancelable)
            {
                unwrap(this).initEvent(
                    String(type),
                    Boolean(bubbles),
                    Boolean(cancelable));
            },

        },
    });
});

//
// Dictionary EventInit
//

function EventInit(o) {
    var rv = O.create(EventInit.prototype);
    if ('bubbles' in o) rv['bubbles'] = Boolean(o['bubbles']);
    if ('cancelable' in o) rv['cancelable'] = Boolean(o['cancelable']);
    return rv;
}
function OptionalEventInit(o) {
    return (o === undefined) ? undefined : EventInit(o);
}
EventInit.prototype = {};

//
// Interface CustomEvent
//

defineLazyProperty(global, "CustomEvent", function() {
    return idl.CustomEvent.publicInterface;
}, true);

defineLazyProperty(idl, "CustomEvent", function() {
    return new IDLInterface({
        name: "CustomEvent",
        superclass: idl.Event,
        constructor: function CustomEvent(
                                type,
                                eventInitDict)
        {
            return wrap(new impl.CustomEvent(
                               String(type),
                               OptionalCustomEventInit(eventInitDict)));
        },
        members: {
            get detail() {
                return unwrap(this).detail;
            },

        },
    });
});

//
// Dictionary CustomEventInit
//

function CustomEventInit(o) {
    var rv = O.create(CustomEventInit.prototype);
    if ('bubbles' in o) rv['bubbles'] = Boolean(o['bubbles']);
    if ('cancelable' in o) rv['cancelable'] = Boolean(o['cancelable']);
    if ('detail' in o) rv['detail'] = o['detail'];
    return rv;
}
function OptionalCustomEventInit(o) {
    return (o === undefined) ? undefined : CustomEventInit(o);
}
CustomEventInit.prototype = O.create(EventInit.prototype);

//
// Interface EventTarget
//

defineLazyProperty(global, "EventTarget", function() {
    return idl.EventTarget.publicInterface;
}, true);

defineLazyProperty(idl, "EventTarget", function() {
    return new IDLInterface({
        name: "EventTarget",
        members: {
            addEventListener: function addEventListener(
                                    type,
                                    listener,
                                    capture)
            {
                unwrap(this).addEventListener(
                    String(type),
                    toCallbackOrNull(listener),
                    OptionalBoolean(capture));
            },

            removeEventListener: function removeEventListener(
                                    type,
                                    listener,
                                    capture)
            {
                unwrap(this).removeEventListener(
                    String(type),
                    toCallbackOrNull(listener),
                    OptionalBoolean(capture));
            },

            dispatchEvent: function dispatchEvent(event) {
                return unwrap(this).dispatchEvent(unwrap(event));
            },

        },
    });
});

//
// Interface Node
//

// Constants defined by Node
const ELEMENT_NODE = 1;
const ATTRIBUTE_NODE = 2;
const TEXT_NODE = 3;
const CDATA_SECTION_NODE = 4;
const ENTITY_REFERENCE_NODE = 5;
const ENTITY_NODE = 6;
const PROCESSING_INSTRUCTION_NODE = 7;
const COMMENT_NODE = 8;
const DOCUMENT_NODE = 9;
const DOCUMENT_TYPE_NODE = 10;
const DOCUMENT_FRAGMENT_NODE = 11;
const NOTATION_NODE = 12;
const DOCUMENT_POSITION_DISCONNECTED = 0x01;
const DOCUMENT_POSITION_PRECEDING = 0x02;
const DOCUMENT_POSITION_FOLLOWING = 0x04;
const DOCUMENT_POSITION_CONTAINS = 0x08;
const DOCUMENT_POSITION_CONTAINED_BY = 0x10;
const DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20;

defineLazyProperty(global, "Node", function() {
    return idl.Node.publicInterface;
}, true);

defineLazyProperty(idl, "Node", function() {
    return new IDLInterface({
        name: "Node",
        superclass: idl.EventTarget,
        constants: {
            ELEMENT_NODE: ELEMENT_NODE,
            ATTRIBUTE_NODE: ATTRIBUTE_NODE,
            TEXT_NODE: TEXT_NODE,
            CDATA_SECTION_NODE: CDATA_SECTION_NODE,
            ENTITY_REFERENCE_NODE: ENTITY_REFERENCE_NODE,
            ENTITY_NODE: ENTITY_NODE,
            PROCESSING_INSTRUCTION_NODE: PROCESSING_INSTRUCTION_NODE,
            COMMENT_NODE: COMMENT_NODE,
            DOCUMENT_NODE: DOCUMENT_NODE,
            DOCUMENT_TYPE_NODE: DOCUMENT_TYPE_NODE,
            DOCUMENT_FRAGMENT_NODE: DOCUMENT_FRAGMENT_NODE,
            NOTATION_NODE: NOTATION_NODE,
            DOCUMENT_POSITION_DISCONNECTED: DOCUMENT_POSITION_DISCONNECTED,
            DOCUMENT_POSITION_PRECEDING: DOCUMENT_POSITION_PRECEDING,
            DOCUMENT_POSITION_FOLLOWING: DOCUMENT_POSITION_FOLLOWING,
            DOCUMENT_POSITION_CONTAINS: DOCUMENT_POSITION_CONTAINS,
            DOCUMENT_POSITION_CONTAINED_BY: DOCUMENT_POSITION_CONTAINED_BY,
            DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC: DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC,
        },
        members: {
            get nodeType() {
                return unwrap(this).nodeType;
            },

            get nodeName() {
                return unwrap(this).nodeName;
            },

            get baseURI() {
                return unwrap(this).baseURI;
            },

            get ownerDocument() {
                return wrap(unwrap(this).ownerDocument);
            },

            get parentNode() {
                return wrap(unwrap(this).parentNode);
            },

            get parentElement() {
                return wrap(unwrap(this).parentElement);
            },

            hasChildNodes: function hasChildNodes() {
                return unwrap(this).hasChildNodes();
            },

            get childNodes() {
                return wrap(unwrap(this).childNodes);
            },

            get firstChild() {
                return wrap(unwrap(this).firstChild);
            },

            get lastChild() {
                return wrap(unwrap(this).lastChild);
            },

            get previousSibling() {
                return wrap(unwrap(this).previousSibling);
            },

            get nextSibling() {
                return wrap(unwrap(this).nextSibling);
            },

            compareDocumentPosition: function compareDocumentPosition(other) {
                return unwrap(this).compareDocumentPosition(unwrap(other));
            },

            get nodeValue() {
                return unwrap(this).nodeValue;
            },
            set nodeValue(newval) {
                unwrap(this).nodeValue = StringOrNull(newval);
            },

            get textContent() {
                return unwrap(this).textContent;
            },
            set textContent(newval) {
                unwrap(this).textContent = StringOrNull(newval);
            },

            insertBefore: function insertBefore(
                                    newChild,
                                    refChild)
            {
                return wrap(unwrap(this).insertBefore(
                    unwrap(newChild),
                    unwrapOrNull(refChild)));
            },

            replaceChild: function replaceChild(
                                    newChild,
                                    oldChild)
            {
                return wrap(unwrap(this).replaceChild(
                    unwrap(newChild),
                    unwrap(oldChild)));
            },

            removeChild: function removeChild(oldChild) {
                return wrap(unwrap(this).removeChild(unwrap(oldChild)));
            },

            appendChild: function appendChild(newChild) {
                return wrap(unwrap(this).appendChild(unwrap(newChild)));
            },

            cloneNode: function cloneNode(deep) {
                return wrap(unwrap(this).cloneNode(Boolean(deep)));
            },

            isSameNode: function isSameNode(node) {
                return unwrap(this).isSameNode(unwrapOrNull(node));
            },

            isEqualNode: function isEqualNode(node) {
                return unwrap(this).isEqualNode(unwrapOrNull(node));
            },

            lookupPrefix: function lookupPrefix(namespace) {
                return unwrap(this).lookupPrefix(StringOrEmpty(namespace));
            },

            lookupNamespaceURI: function lookupNamespaceURI(prefix) {
                return unwrap(this).lookupNamespaceURI(StringOrNull(prefix));
            },

            isDefaultNamespace: function isDefaultNamespace(namespace) {
                return unwrap(this).isDefaultNamespace(StringOrEmpty(namespace));
            },

        },
    });
});

//
// Interface DocumentFragment
//

defineLazyProperty(global, "DocumentFragment", function() {
    return idl.DocumentFragment.publicInterface;
}, true);

defineLazyProperty(idl, "DocumentFragment", function() {
    return new IDLInterface({
        name: "DocumentFragment",
        superclass: idl.Node,
        members: {
        },
    });
});

//
// Interface Document
//

defineLazyProperty(global, "Document", function() {
    return idl.Document.publicInterface;
}, true);

defineLazyProperty(idl, "Document", function() {
    return new IDLInterface({
        name: "Document",
        superclass: idl.Node,
        members: {
            _setMutationHandler: function _setMutationHandler(handler) {
                unwrap(this)._setMutationHandler(toCallback(handler));
            },

            _dispatchRendererEvent: function _dispatchRendererEvent(
                                    target,
                                    type,
                                    details)
            {
                unwrap(this)._dispatchRendererEvent(
                    toULong(target),
                    String(type),
                    EventInit(details));
            },

            get implementation() {
                return wrap(unwrap(this).implementation);
            },

            get documentURI() {
                return unwrap(this).documentURI;
            },
            set documentURI(newval) {
                unwrap(this).documentURI = String(newval);
            },

            get compatMode() {
                return unwrap(this).compatMode;
            },

            get doctype() {
                return wrap(unwrap(this).doctype);
            },

            get documentElement() {
                return wrap(unwrap(this).documentElement);
            },

            getElementsByTagName: function getElementsByTagName(qualifiedName) {
                return wrap(unwrap(this).getElementsByTagName(String(qualifiedName)));
            },

            getElementsByTagNameNS: function getElementsByTagNameNS(
                                    namespace,
                                    localName)
            {
                return wrap(unwrap(this).getElementsByTagNameNS(
                    String(namespace),
                    String(localName)));
            },

            getElementsByClassName: function getElementsByClassName(classNames) {
                return wrap(unwrap(this).getElementsByClassName(String(classNames)));
            },

            getElementById: function getElementById(elementId) {
                return wrap(unwrap(this).getElementById(String(elementId)));
            },

            createElement: function createElement(localName) {
                return wrap(unwrap(this).createElement(StringOrEmpty(localName)));
            },

            createElementNS: function createElementNS(
                                    namespace,
                                    qualifiedName)
            {
                return wrap(unwrap(this).createElementNS(
                    String(namespace),
                    String(qualifiedName)));
            },

            createDocumentFragment: function createDocumentFragment() {
                return wrap(unwrap(this).createDocumentFragment());
            },

            createTextNode: function createTextNode(data) {
                return wrap(unwrap(this).createTextNode(String(data)));
            },

            createComment: function createComment(data) {
                return wrap(unwrap(this).createComment(String(data)));
            },

            createProcessingInstruction: function createProcessingInstruction(
                                    target,
                                    data)
            {
                return wrap(unwrap(this).createProcessingInstruction(
                    String(target),
                    String(data)));
            },

            importNode: function importNode(
                                    node,
                                    deep)
            {
                return wrap(unwrap(this).importNode(
                    unwrap(node),
                    Boolean(deep)));
            },

            adoptNode: function adoptNode(node) {
                return wrap(unwrap(this).adoptNode(unwrap(node)));
            },

            createEvent: function createEvent(eventInterfaceName) {
                return wrap(unwrap(this).createEvent(String(eventInterfaceName)));
            },

            get URL() {
                return unwrap(this).URL;
            },

            get domain() {
                return unwrap(this).domain;
            },
            set domain(newval) {
                unwrap(this).domain = String(newval);
            },

            get referrer() {
                return unwrap(this).referrer;
            },

            get cookie() {
                return unwrap(this).cookie;
            },
            set cookie(newval) {
                unwrap(this).cookie = String(newval);
            },

            get lastModified() {
                return unwrap(this).lastModified;
            },

            get readyState() {
                return unwrap(this).readyState;
            },

            get title() {
                return unwrap(this).title;
            },
            set title(newval) {
                unwrap(this).title = String(newval);
            },

            get dir() {
                return unwrap(this).dir;
            },
            set dir(newval) {
                unwrap(this).dir = String(newval);
            },

            get body() {
                return wrap(unwrap(this).body);
            },
            set body(newval) {
                unwrap(this).body = unwrapOrNull(newval);
            },

            get head() {
                return wrap(unwrap(this).head);
            },

            get images() {
                return wrap(unwrap(this).images);
            },

            get embeds() {
                return wrap(unwrap(this).embeds);
            },

            get plugins() {
                return wrap(unwrap(this).plugins);
            },

            get links() {
                return wrap(unwrap(this).links);
            },

            get forms() {
                return wrap(unwrap(this).forms);
            },

            get scripts() {
                return wrap(unwrap(this).scripts);
            },

            getElementsByName: function getElementsByName(elementName) {
                return wrap(unwrap(this).getElementsByName(String(elementName)));
            },

            get innerHTML() {
                return unwrap(this).innerHTML;
            },
            set innerHTML(newval) {
                unwrap(this).innerHTML = String(newval);
            },

            write: function write(text /*...*/) {
                var context = unwrap(this);
                var args = [];
                for(var i = 0; i < arguments.length; i++) {
                    push(args, String(arguments[i]));
                }
                apply(context.write, context, args);
            },

            writeln: function writeln(text /*...*/) {
                var context = unwrap(this);
                var args = [];
                for(var i = 0; i < arguments.length; i++) {
                    push(args, String(arguments[i]));
                }
                apply(context.writeln, context, args);
            },

            get defaultView() {
                return wrap(unwrap(this).defaultView);
            },

            get onabort() {
                return unwrap(this).onabort;
            },
            set onabort(newval) {
                unwrap(this).onabort = toCallbackOrNull(newval);
            },

            get onblur() {
                return unwrap(this).onblur;
            },
            set onblur(newval) {
                unwrap(this).onblur = toCallbackOrNull(newval);
            },

            get oncanplay() {
                return unwrap(this).oncanplay;
            },
            set oncanplay(newval) {
                unwrap(this).oncanplay = toCallbackOrNull(newval);
            },

            get oncanplaythrough() {
                return unwrap(this).oncanplaythrough;
            },
            set oncanplaythrough(newval) {
                unwrap(this).oncanplaythrough = toCallbackOrNull(newval);
            },

            get onchange() {
                return unwrap(this).onchange;
            },
            set onchange(newval) {
                unwrap(this).onchange = toCallbackOrNull(newval);
            },

            get onclick() {
                return unwrap(this).onclick;
            },
            set onclick(newval) {
                unwrap(this).onclick = toCallbackOrNull(newval);
            },

            get oncontextmenu() {
                return unwrap(this).oncontextmenu;
            },
            set oncontextmenu(newval) {
                unwrap(this).oncontextmenu = toCallbackOrNull(newval);
            },

            get oncuechange() {
                return unwrap(this).oncuechange;
            },
            set oncuechange(newval) {
                unwrap(this).oncuechange = toCallbackOrNull(newval);
            },

            get ondblclick() {
                return unwrap(this).ondblclick;
            },
            set ondblclick(newval) {
                unwrap(this).ondblclick = toCallbackOrNull(newval);
            },

            get ondrag() {
                return unwrap(this).ondrag;
            },
            set ondrag(newval) {
                unwrap(this).ondrag = toCallbackOrNull(newval);
            },

            get ondragend() {
                return unwrap(this).ondragend;
            },
            set ondragend(newval) {
                unwrap(this).ondragend = toCallbackOrNull(newval);
            },

            get ondragenter() {
                return unwrap(this).ondragenter;
            },
            set ondragenter(newval) {
                unwrap(this).ondragenter = toCallbackOrNull(newval);
            },

            get ondragleave() {
                return unwrap(this).ondragleave;
            },
            set ondragleave(newval) {
                unwrap(this).ondragleave = toCallbackOrNull(newval);
            },

            get ondragover() {
                return unwrap(this).ondragover;
            },
            set ondragover(newval) {
                unwrap(this).ondragover = toCallbackOrNull(newval);
            },

            get ondragstart() {
                return unwrap(this).ondragstart;
            },
            set ondragstart(newval) {
                unwrap(this).ondragstart = toCallbackOrNull(newval);
            },

            get ondrop() {
                return unwrap(this).ondrop;
            },
            set ondrop(newval) {
                unwrap(this).ondrop = toCallbackOrNull(newval);
            },

            get ondurationchange() {
                return unwrap(this).ondurationchange;
            },
            set ondurationchange(newval) {
                unwrap(this).ondurationchange = toCallbackOrNull(newval);
            },

            get onemptied() {
                return unwrap(this).onemptied;
            },
            set onemptied(newval) {
                unwrap(this).onemptied = toCallbackOrNull(newval);
            },

            get onended() {
                return unwrap(this).onended;
            },
            set onended(newval) {
                unwrap(this).onended = toCallbackOrNull(newval);
            },

            get onerror() {
                return unwrap(this).onerror;
            },
            set onerror(newval) {
                unwrap(this).onerror = toCallbackOrNull(newval);
            },

            get onfocus() {
                return unwrap(this).onfocus;
            },
            set onfocus(newval) {
                unwrap(this).onfocus = toCallbackOrNull(newval);
            },

            get oninput() {
                return unwrap(this).oninput;
            },
            set oninput(newval) {
                unwrap(this).oninput = toCallbackOrNull(newval);
            },

            get oninvalid() {
                return unwrap(this).oninvalid;
            },
            set oninvalid(newval) {
                unwrap(this).oninvalid = toCallbackOrNull(newval);
            },

            get onkeydown() {
                return unwrap(this).onkeydown;
            },
            set onkeydown(newval) {
                unwrap(this).onkeydown = toCallbackOrNull(newval);
            },

            get onkeypress() {
                return unwrap(this).onkeypress;
            },
            set onkeypress(newval) {
                unwrap(this).onkeypress = toCallbackOrNull(newval);
            },

            get onkeyup() {
                return unwrap(this).onkeyup;
            },
            set onkeyup(newval) {
                unwrap(this).onkeyup = toCallbackOrNull(newval);
            },

            get onload() {
                return unwrap(this).onload;
            },
            set onload(newval) {
                unwrap(this).onload = toCallbackOrNull(newval);
            },

            get onloadeddata() {
                return unwrap(this).onloadeddata;
            },
            set onloadeddata(newval) {
                unwrap(this).onloadeddata = toCallbackOrNull(newval);
            },

            get onloadedmetadata() {
                return unwrap(this).onloadedmetadata;
            },
            set onloadedmetadata(newval) {
                unwrap(this).onloadedmetadata = toCallbackOrNull(newval);
            },

            get onloadstart() {
                return unwrap(this).onloadstart;
            },
            set onloadstart(newval) {
                unwrap(this).onloadstart = toCallbackOrNull(newval);
            },

            get onmousedown() {
                return unwrap(this).onmousedown;
            },
            set onmousedown(newval) {
                unwrap(this).onmousedown = toCallbackOrNull(newval);
            },

            get onmousemove() {
                return unwrap(this).onmousemove;
            },
            set onmousemove(newval) {
                unwrap(this).onmousemove = toCallbackOrNull(newval);
            },

            get onmouseout() {
                return unwrap(this).onmouseout;
            },
            set onmouseout(newval) {
                unwrap(this).onmouseout = toCallbackOrNull(newval);
            },

            get onmouseover() {
                return unwrap(this).onmouseover;
            },
            set onmouseover(newval) {
                unwrap(this).onmouseover = toCallbackOrNull(newval);
            },

            get onmouseup() {
                return unwrap(this).onmouseup;
            },
            set onmouseup(newval) {
                unwrap(this).onmouseup = toCallbackOrNull(newval);
            },

            get onmousewheel() {
                return unwrap(this).onmousewheel;
            },
            set onmousewheel(newval) {
                unwrap(this).onmousewheel = toCallbackOrNull(newval);
            },

            get onpause() {
                return unwrap(this).onpause;
            },
            set onpause(newval) {
                unwrap(this).onpause = toCallbackOrNull(newval);
            },

            get onplay() {
                return unwrap(this).onplay;
            },
            set onplay(newval) {
                unwrap(this).onplay = toCallbackOrNull(newval);
            },

            get onplaying() {
                return unwrap(this).onplaying;
            },
            set onplaying(newval) {
                unwrap(this).onplaying = toCallbackOrNull(newval);
            },

            get onprogress() {
                return unwrap(this).onprogress;
            },
            set onprogress(newval) {
                unwrap(this).onprogress = toCallbackOrNull(newval);
            },

            get onratechange() {
                return unwrap(this).onratechange;
            },
            set onratechange(newval) {
                unwrap(this).onratechange = toCallbackOrNull(newval);
            },

            get onreadystatechange() {
                return unwrap(this).onreadystatechange;
            },
            set onreadystatechange(newval) {
                unwrap(this).onreadystatechange = toCallbackOrNull(newval);
            },

            get onreset() {
                return unwrap(this).onreset;
            },
            set onreset(newval) {
                unwrap(this).onreset = toCallbackOrNull(newval);
            },

            get onscroll() {
                return unwrap(this).onscroll;
            },
            set onscroll(newval) {
                unwrap(this).onscroll = toCallbackOrNull(newval);
            },

            get onseeked() {
                return unwrap(this).onseeked;
            },
            set onseeked(newval) {
                unwrap(this).onseeked = toCallbackOrNull(newval);
            },

            get onseeking() {
                return unwrap(this).onseeking;
            },
            set onseeking(newval) {
                unwrap(this).onseeking = toCallbackOrNull(newval);
            },

            get onselect() {
                return unwrap(this).onselect;
            },
            set onselect(newval) {
                unwrap(this).onselect = toCallbackOrNull(newval);
            },

            get onshow() {
                return unwrap(this).onshow;
            },
            set onshow(newval) {
                unwrap(this).onshow = toCallbackOrNull(newval);
            },

            get onstalled() {
                return unwrap(this).onstalled;
            },
            set onstalled(newval) {
                unwrap(this).onstalled = toCallbackOrNull(newval);
            },

            get onsubmit() {
                return unwrap(this).onsubmit;
            },
            set onsubmit(newval) {
                unwrap(this).onsubmit = toCallbackOrNull(newval);
            },

            get onsuspend() {
                return unwrap(this).onsuspend;
            },
            set onsuspend(newval) {
                unwrap(this).onsuspend = toCallbackOrNull(newval);
            },

            get ontimeupdate() {
                return unwrap(this).ontimeupdate;
            },
            set ontimeupdate(newval) {
                unwrap(this).ontimeupdate = toCallbackOrNull(newval);
            },

            get onvolumechange() {
                return unwrap(this).onvolumechange;
            },
            set onvolumechange(newval) {
                unwrap(this).onvolumechange = toCallbackOrNull(newval);
            },

            get onwaiting() {
                return unwrap(this).onwaiting;
            },
            set onwaiting(newval) {
                unwrap(this).onwaiting = toCallbackOrNull(newval);
            },

        },
    });
});

//
// Interface DOMImplementation
//

defineLazyProperty(global, "DOMImplementation", function() {
    return idl.DOMImplementation.publicInterface;
}, true);

defineLazyProperty(idl, "DOMImplementation", function() {
    return new IDLInterface({
        name: "DOMImplementation",
        members: {
            hasFeature: function hasFeature(
                                    feature,
                                    version)
            {
                return unwrap(this).hasFeature(
                    String(feature),
                    StringOrEmpty(version));
            },

            createDocumentType: function createDocumentType(
                                    qualifiedName,
                                    publicId,
                                    systemId)
            {
                return wrap(unwrap(this).createDocumentType(
                    StringOrEmpty(qualifiedName),
                    String(publicId),
                    String(systemId)));
            },

            createDocument: function createDocument(
                                    namespace,
                                    qualifiedName,
                                    doctype)
            {
                return wrap(unwrap(this).createDocument(
                    StringOrEmpty(namespace),
                    StringOrEmpty(qualifiedName),
                    unwrapOrNull(doctype)));
            },

            createHTMLDocument: function createHTMLDocument(title) {
                return wrap(unwrap(this).createHTMLDocument(String(title)));
            },

            mozSetOutputMutationHandler: function mozSetOutputMutationHandler(
                                    doc,
                                    handler)
            {
                unwrap(this).mozSetOutputMutationHandler(
                    unwrap(doc),
                    toCallback(handler));
            },

            mozGetInputMutationHandler: function mozGetInputMutationHandler(doc) {
                return unwrap(this).mozGetInputMutationHandler(unwrap(doc));
            },

            get mozHTMLParser() {
                return unwrap(this).mozHTMLParser;
            },

        },
    });
});

//
// Interface Element
//

defineLazyProperty(global, "Element", function() {
    return idl.Element.publicInterface;
}, true);

defineLazyProperty(idl, "Element", function() {
    return new IDLInterface({
        name: "Element",
        superclass: idl.Node,
        members: {
            get namespaceURI() {
                return unwrap(this).namespaceURI;
            },

            get prefix() {
                return unwrap(this).prefix;
            },

            get localName() {
                return unwrap(this).localName;
            },

            get tagName() {
                return unwrap(this).tagName;
            },

            get attributes() {
                return wrap(unwrap(this).attributes);
            },

            getAttribute: function getAttribute(qualifiedName) {
                return unwrap(this).getAttribute(String(qualifiedName));
            },

            getAttributeNS: function getAttributeNS(
                                    namespace,
                                    localName)
            {
                return unwrap(this).getAttributeNS(
                    String(namespace),
                    String(localName));
            },

            setAttribute: function setAttribute(
                                    qualifiedName,
                                    value)
            {
                unwrap(this).setAttribute(
                    String(qualifiedName),
                    String(value));
            },

            setAttributeNS: function setAttributeNS(
                                    namespace,
                                    qualifiedName,
                                    value)
            {
                unwrap(this).setAttributeNS(
                    String(namespace),
                    String(qualifiedName),
                    String(value));
            },

            removeAttribute: function removeAttribute(qualifiedName) {
                unwrap(this).removeAttribute(String(qualifiedName));
            },

            removeAttributeNS: function removeAttributeNS(
                                    namespace,
                                    localName)
            {
                unwrap(this).removeAttributeNS(
                    String(namespace),
                    String(localName));
            },

            hasAttribute: function hasAttribute(qualifiedName) {
                return unwrap(this).hasAttribute(String(qualifiedName));
            },

            hasAttributeNS: function hasAttributeNS(
                                    namespace,
                                    localName)
            {
                return unwrap(this).hasAttributeNS(
                    String(namespace),
                    String(localName));
            },

            getElementsByTagName: function getElementsByTagName(qualifiedName) {
                return wrap(unwrap(this).getElementsByTagName(String(qualifiedName)));
            },

            getElementsByTagNameNS: function getElementsByTagNameNS(
                                    namespace,
                                    localName)
            {
                return wrap(unwrap(this).getElementsByTagNameNS(
                    String(namespace),
                    String(localName)));
            },

            getElementsByClassName: function getElementsByClassName(classNames) {
                return wrap(unwrap(this).getElementsByClassName(String(classNames)));
            },

            get children() {
                return wrap(unwrap(this).children);
            },

            get firstElementChild() {
                return wrap(unwrap(this).firstElementChild);
            },

            get lastElementChild() {
                return wrap(unwrap(this).lastElementChild);
            },

            get previousElementSibling() {
                return wrap(unwrap(this).previousElementSibling);
            },

            get nextElementSibling() {
                return wrap(unwrap(this).nextElementSibling);
            },

            get childElementCount() {
                return unwrap(this).childElementCount;
            },

        },
    });
});

//
// Interface Attr
//

defineLazyProperty(global, "Attr", function() {
    return idl.Attr.publicInterface;
}, true);

defineLazyProperty(idl, "Attr", function() {
    return new IDLInterface({
        name: "Attr",
        members: {
            get namespaceURI() {
                return unwrap(this).namespaceURI;
            },

            get prefix() {
                return unwrap(this).prefix;
            },

            get localName() {
                return unwrap(this).localName;
            },

            get name() {
                return unwrap(this).name;
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

        },
    });
});

//
// Interface DocumentType
//

defineLazyProperty(global, "DocumentType", function() {
    return idl.DocumentType.publicInterface;
}, true);

defineLazyProperty(idl, "DocumentType", function() {
    return new IDLInterface({
        name: "DocumentType",
        superclass: idl.Node,
        members: {
            get name() {
                return unwrap(this).name;
            },

            get publicId() {
                return unwrap(this).publicId;
            },

            get systemId() {
                return unwrap(this).systemId;
            },

        },
    });
});

//
// Interface ProcessingInstruction
//

defineLazyProperty(global, "ProcessingInstruction", function() {
    return idl.ProcessingInstruction.publicInterface;
}, true);

defineLazyProperty(idl, "ProcessingInstruction", function() {
    return new IDLInterface({
        name: "ProcessingInstruction",
        superclass: idl.Node,
        members: {
            get target() {
                return unwrap(this).target;
            },

            get data() {
                return unwrap(this).data;
            },
            set data(newval) {
                unwrap(this).data = String(newval);
            },

        },
    });
});

//
// Interface CharacterData
//

defineLazyProperty(global, "CharacterData", function() {
    return idl.CharacterData.publicInterface;
}, true);

defineLazyProperty(idl, "CharacterData", function() {
    return new IDLInterface({
        name: "CharacterData",
        superclass: idl.Node,
        members: {
            get data() {
                return unwrap(this).data;
            },
            set data(newval) {
                unwrap(this).data = StringOrEmpty(newval);
            },

            get length() {
                return unwrap(this).length;
            },

            substringData: function substringData(
                                    offset,
                                    count)
            {
                return unwrap(this).substringData(
                    toULong(offset),
                    toULong(count));
            },

            appendData: function appendData(data) {
                unwrap(this).appendData(String(data));
            },

            insertData: function insertData(
                                    offset,
                                    data)
            {
                unwrap(this).insertData(
                    toULong(offset),
                    String(data));
            },

            deleteData: function deleteData(
                                    offset,
                                    count)
            {
                unwrap(this).deleteData(
                    toULong(offset),
                    toULong(count));
            },

            replaceData: function replaceData(
                                    offset,
                                    count,
                                    data)
            {
                unwrap(this).replaceData(
                    toULong(offset),
                    toULong(count),
                    String(data));
            },

        },
    });
});

//
// Interface Text
//

defineLazyProperty(global, "Text", function() {
    return idl.Text.publicInterface;
}, true);

defineLazyProperty(idl, "Text", function() {
    return new IDLInterface({
        name: "Text",
        superclass: idl.CharacterData,
        members: {
            splitText: function splitText(offset) {
                return wrap(unwrap(this).splitText(toULong(offset)));
            },

            get wholeText() {
                return unwrap(this).wholeText;
            },

            replaceWholeText: function replaceWholeText(data) {
                return wrap(unwrap(this).replaceWholeText(String(data)));
            },

        },
    });
});

//
// Interface Comment
//

defineLazyProperty(global, "Comment", function() {
    return idl.Comment.publicInterface;
}, true);

defineLazyProperty(idl, "Comment", function() {
    return new IDLInterface({
        name: "Comment",
        superclass: idl.CharacterData,
        members: {
        },
    });
});

//
// Interface NodeList
//

defineLazyProperty(global, "NodeList", function() {
    return idl.NodeList.publicInterface;
}, true);

defineLazyProperty(idl, "NodeList", function() {
    return new IDLInterface({
        name: "NodeList",
        proxyFactory: NodeListProxy,
        members: {
            item: function item(index) {
                return wrap(unwrap(this).item(toULong(index)));
            },

            get length() {
                return unwrap(this).length;
            },

        },
    });
});

//
// Interface HTMLCollection
//

defineLazyProperty(global, "HTMLCollection", function() {
    return idl.HTMLCollection.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLCollection", function() {
    return new IDLInterface({
        name: "HTMLCollection",
        proxyFactory: HTMLCollectionProxy,
        members: {
            get length() {
                return unwrap(this).length;
            },

            item: function item(index) {
                return wrap(unwrap(this).item(toULong(index)));
            },

            namedItem: function namedItem(name) {
                return wrap(unwrap(this).namedItem(String(name)));
            },

        },
    });
});

//
// Interface DOMStringList
//

defineLazyProperty(global, "DOMStringList", function() {
    return idl.DOMStringList.publicInterface;
}, true);

defineLazyProperty(idl, "DOMStringList", function() {
    return new IDLInterface({
        name: "DOMStringList",
        members: {
            get length() {
                return unwrap(this).length;
            },

            item: function item(index) {
                return unwrap(this).item(toULong(index));
            },

            contains: function contains(string) {
                return unwrap(this).contains(String(string));
            },

        },
    });
});

//
// Interface DOMTokenList
//

defineLazyProperty(global, "DOMTokenList", function() {
    return idl.DOMTokenList.publicInterface;
}, true);

defineLazyProperty(idl, "DOMTokenList", function() {
    return new IDLInterface({
        name: "DOMTokenList",
        members: {
            get length() {
                return unwrap(this).length;
            },

            item: function item(index) {
                return unwrap(this).item(toULong(index));
            },

            contains: function contains(token) {
                return unwrap(this).contains(String(token));
            },

            add: function add(token) {
                unwrap(this).add(String(token));
            },

            remove: function remove(token) {
                unwrap(this).remove(String(token));
            },

            toggle: function toggle(token) {
                return unwrap(this).toggle(String(token));
            },

            toString: function toString() {
                return unwrap(this).toString();
            },

        },
    });
});

//
// Interface DOMSettableTokenList
//

defineLazyProperty(global, "DOMSettableTokenList", function() {
    return idl.DOMSettableTokenList.publicInterface;
}, true);

defineLazyProperty(idl, "DOMSettableTokenList", function() {
    return new IDLInterface({
        name: "DOMSettableTokenList",
        superclass: idl.DOMTokenList,
        members: {
            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

        },
    });
});

defineLazyProperty(idl, "AttrArray", function() {
    return new IDLInterface({
        name: "AttrArray",
        proxyFactory: AttrArrayProxy,
    });
});



/************************************************************************
 *  src/events.js
 ************************************************************************/

//@line 1 "src/events.js"
//
// DO NOT EDIT.
// This file was generated by idl2domjs from src/idl/events.idl
//


//
// Interface UIEvent
//

defineLazyProperty(global, "UIEvent", function() {
    return idl.UIEvent.publicInterface;
}, true);

defineLazyProperty(idl, "UIEvent", function() {
    return new IDLInterface({
        name: "UIEvent",
        superclass: idl.Event,
        members: {
            get view() {
                return wrap(unwrap(this).view);
            },

            get detail() {
                return unwrap(this).detail;
            },

            initUIEvent: function initUIEvent(
                                    typeArg,
                                    canBubbleArg,
                                    cancelableArg,
                                    viewArg,
                                    detailArg)
            {
                unwrap(this).initUIEvent(
                    String(typeArg),
                    Boolean(canBubbleArg),
                    Boolean(cancelableArg),
                    unwrap(viewArg),
                    toLong(detailArg));
            },

        },
    });
});

//
// Interface MouseEvent
//

defineLazyProperty(global, "MouseEvent", function() {
    return idl.MouseEvent.publicInterface;
}, true);

defineLazyProperty(idl, "MouseEvent", function() {
    return new IDLInterface({
        name: "MouseEvent",
        superclass: idl.UIEvent,
        members: {
            get screenX() {
                return unwrap(this).screenX;
            },

            get screenY() {
                return unwrap(this).screenY;
            },

            get clientX() {
                return unwrap(this).clientX;
            },

            get clientY() {
                return unwrap(this).clientY;
            },

            get ctrlKey() {
                return unwrap(this).ctrlKey;
            },

            get shiftKey() {
                return unwrap(this).shiftKey;
            },

            get altKey() {
                return unwrap(this).altKey;
            },

            get metaKey() {
                return unwrap(this).metaKey;
            },

            get button() {
                return unwrap(this).button;
            },

            get buttons() {
                return unwrap(this).buttons;
            },

            get relatedTarget() {
                return wrap(unwrap(this).relatedTarget);
            },

            initMouseEvent: function initMouseEvent(
                                    typeArg,
                                    canBubbleArg,
                                    cancelableArg,
                                    viewArg,
                                    detailArg,
                                    screenXArg,
                                    screenYArg,
                                    clientXArg,
                                    clientYArg,
                                    ctrlKeyArg,
                                    altKeyArg,
                                    shiftKeyArg,
                                    metaKeyArg,
                                    buttonArg,
                                    relatedTargetArg)
            {
                unwrap(this).initMouseEvent(
                    String(typeArg),
                    Boolean(canBubbleArg),
                    Boolean(cancelableArg),
                    unwrap(viewArg),
                    toLong(detailArg),
                    toLong(screenXArg),
                    toLong(screenYArg),
                    toLong(clientXArg),
                    toLong(clientYArg),
                    Boolean(ctrlKeyArg),
                    Boolean(altKeyArg),
                    Boolean(shiftKeyArg),
                    Boolean(metaKeyArg),
                    toUShort(buttonArg),
                    unwrap(relatedTargetArg));
            },

            getModifierState: function getModifierState(keyArg) {
                return unwrap(this).getModifierState(String(keyArg));
            },

        },
    });
});

//
// Interface WheelEvent
//

defineLazyProperty(global, "WheelEvent", function() {
    return idl.WheelEvent.publicInterface;
}, true);

defineLazyProperty(idl, "WheelEvent", function() {
    return new IDLInterface({
        name: "WheelEvent",
        superclass: idl.MouseEvent,
        members: {
            get deltaX() {
                return unwrap(this).deltaX;
            },

            get deltaY() {
                return unwrap(this).deltaY;
            },

            get deltaZ() {
                return unwrap(this).deltaZ;
            },

            initWheelEvent: function initWheelEvent(
                                    typeArg,
                                    canBubbleArg,
                                    cancelableArg,
                                    viewArg,
                                    detailArg,
                                    screenXArg,
                                    screenYArg,
                                    clientXArg,
                                    clientYArg,
                                    deltaXArg,
                                    deltaYArg,
                                    deltaZArg,
                                    ctrlKeyArg,
                                    altKeyArg,
                                    shiftKeyArg,
                                    metaKeyArg,
                                    buttonArg,
                                    relatedTargetArg)
            {
                unwrap(this).initWheelEvent(
                    String(typeArg),
                    Boolean(canBubbleArg),
                    Boolean(cancelableArg),
                    unwrap(viewArg),
                    toLong(detailArg),
                    toLong(screenXArg),
                    toLong(screenYArg),
                    toLong(clientXArg),
                    toLong(clientYArg),
                    toLong(deltaXArg),
                    toLong(deltaYArg),
                    toLong(deltaZArg),
                    Boolean(ctrlKeyArg),
                    Boolean(altKeyArg),
                    Boolean(shiftKeyArg),
                    Boolean(metaKeyArg),
                    toUShort(buttonArg),
                    unwrap(relatedTargetArg));
            },

            getModifierState: function getModifierState(keyArg) {
                return unwrap(this).getModifierState(String(keyArg));
            },

        },
    });
});

//
// Interface KeyboardEvent
//

defineLazyProperty(global, "KeyboardEvent", function() {
    return idl.KeyboardEvent.publicInterface;
}, true);

defineLazyProperty(idl, "KeyboardEvent", function() {
    return new IDLInterface({
        name: "KeyboardEvent",
        superclass: idl.UIEvent,
        members: {
            get key() {
                return unwrap(this).key;
            },

            get keyCode() {
                return unwrap(this).keyCode;
            },

            initKeyboardEvent: function initKeyboardEvent(
                                    type,
                                    key)
            {
                unwrap(this).initKeyboardEvent(
                    String(type),
                    String(key));
            },

        },
    });
});

//
// Interface VoiceEvent
//

defineLazyProperty(global, "VoiceEvent", function() {
    return idl.VoiceEvent.publicInterface;
}, true);

defineLazyProperty(idl, "VoiceEvent", function() {
    return new IDLInterface({
        name: "VoiceEvent",
        superclass: idl.UIEvent,
        members: {
            get command() {
                return unwrap(this).command;
            },

            get confidence() {
                return unwrap(this).confidence;
            },

            get type() {
                return unwrap(this).type;
            },

            VoiceEvent: function VoiceEvent(
                                    type,
                                    command,
                                    confidence)
            {
                unwrap(this).VoiceEvent(
                    String(type),
                    String(command),
                    toDouble(confidence));
            },

        },
    });
});

//
// Interface SpatialInputEvent
//

defineLazyProperty(global, "SpatialInputEvent", function() {
    return idl.SpatialInputEvent.publicInterface;
}, true);

defineLazyProperty(idl, "SpatialInputEvent", function() {
    return new IDLInterface({
        name: "SpatialInputEvent",
        superclass: idl.UIEvent,
        members: {
            get isPressed() {
                return unwrap(this).isPressed;
            },

            get x() {
                return unwrap(this).x;
            },

            get y() {
                return unwrap(this).y;
            },

            get z() {
                return unwrap(this).z;
            },

            get sourceKind() {
                return unwrap(this).sourceKind;
            },

            initSpatialInputEvent: function initSpatialInputEvent(
                                    type,
                                    isPressedArg,
                                    xArg,
                                    yArg,
                                    zArg,
                                    sourceKindArg)
            {
                unwrap(this).initSpatialInputEvent(
                    String(type),
                    Boolean(isPressedArg),
                    toDouble(xArg),
                    toDouble(yArg),
                    toDouble(zArg),
                    toDouble(sourceKindArg));
            },

        },
    });
});



/************************************************************************
 *  src/htmlelts.js
 ************************************************************************/

//@line 1 "src/htmlelts.js"
//
// DO NOT EDIT.
// This file was generated by idl2domjs from src/idl/htmlelts.idl
//


//
// Interface HTMLFormControlsCollection
//

defineLazyProperty(global, "HTMLFormControlsCollection", function() {
    return idl.HTMLFormControlsCollection.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLFormControlsCollection", function() {
    return new IDLInterface({
        name: "HTMLFormControlsCollection",
        superclass: idl.HTMLCollection,
        proxyFactory: HTMLFormControlsCollectionProxy,
        members: {
            namedItem: function namedItem(name) {
                return unwrap(this).namedItem(String(name));
            },

        },
    });
});

//
// Interface RadioNodeList
//

defineLazyProperty(global, "RadioNodeList", function() {
    return idl.RadioNodeList.publicInterface;
}, true);

defineLazyProperty(idl, "RadioNodeList", function() {
    return new IDLInterface({
        name: "RadioNodeList",
        superclass: idl.NodeList,
        members: {
            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

        },
    });
});

//
// Interface HTMLOptionsCollection
//

defineLazyProperty(global, "HTMLOptionsCollection", function() {
    return idl.HTMLOptionsCollection.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLOptionsCollection", function() {
    return new IDLInterface({
        name: "HTMLOptionsCollection",
        superclass: idl.HTMLCollection,
        proxyFactory: HTMLOptionsCollectionProxy,
        members: {
            get length() {
                return unwrap(this).length;
            },
            set length(newval) {
                unwrap(this).length = toULong(newval);
            },

            namedItem: function namedItem(name) {
                return unwrap(this).namedItem(String(name));
            },

            remove: function remove(index) {
                unwrap(this).remove(toLong(index));
            },

            get selectedIndex() {
                return unwrap(this).selectedIndex;
            },
            set selectedIndex(newval) {
                unwrap(this).selectedIndex = toLong(newval);
            },

        },
    });
});

//
// Interface HTMLPropertiesCollection
//

defineLazyProperty(global, "HTMLPropertiesCollection", function() {
    return idl.HTMLPropertiesCollection.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLPropertiesCollection", function() {
    return new IDLInterface({
        name: "HTMLPropertiesCollection",
        superclass: idl.HTMLCollection,
        proxyFactory: HTMLPropertiesCollectionProxy,
        members: {
            namedItem: function namedItem(name) {
                return wrap(unwrap(this).namedItem(String(name)));
            },

            get names() {
                return wrap(unwrap(this).names);
            },

        },
    });
});

//
// Interface PropertyNodeList
//

defineLazyProperty(global, "PropertyNodeList", function() {
    return idl.PropertyNodeList.publicInterface;
}, true);

defineLazyProperty(idl, "PropertyNodeList", function() {
    return new IDLInterface({
        name: "PropertyNodeList",
        superclass: idl.NodeList,
        members: {
            getValues: function getValues() {
                return wrap(unwrap(this).getValues());
            },

        },
    });
});

//
// Interface DOMStringMap
//

defineLazyProperty(global, "DOMStringMap", function() {
    return idl.DOMStringMap.publicInterface;
}, true);

defineLazyProperty(idl, "DOMStringMap", function() {
    return new IDLInterface({
        name: "DOMStringMap",
        proxyFactory: DOMStringMapProxy,
        members: {
        },
    });
});

//
// Interface DOMElementMap
//

defineLazyProperty(global, "DOMElementMap", function() {
    return idl.DOMElementMap.publicInterface;
}, true);

defineLazyProperty(idl, "DOMElementMap", function() {
    return new IDLInterface({
        name: "DOMElementMap",
        proxyFactory: DOMElementMapProxy,
        members: {
        },
    });
});

//
// Interface CSSStyleDeclaration
//

defineLazyProperty(global, "CSSStyleDeclaration", function() {
    return idl.CSSStyleDeclaration.publicInterface;
}, true);

defineLazyProperty(idl, "CSSStyleDeclaration", function() {
    return new IDLInterface({
        name: "CSSStyleDeclaration",
        members: {
            get cssText() {
                return unwrap(this).cssText;
            },
            set cssText(newval) {
                unwrap(this).cssText = String(newval);
            },

            get length() {
                return unwrap(this).length;
            },

            item: function item(index) {
                return unwrap(this).item(toULong(index));
            },

            getPropertyValue: function getPropertyValue(property) {
                return unwrap(this).getPropertyValue(String(property));
            },

            getPropertyPriority: function getPropertyPriority(property) {
                return unwrap(this).getPropertyPriority(String(property));
            },

            setProperty: function setProperty(
                                    property,
                                    value,
                                    priority)
            {
                unwrap(this).setProperty(
                    String(property),
                    String(value),
                    OptionalString(priority));
            },

            removeProperty: function removeProperty(property) {
                return unwrap(this).removeProperty(String(property));
            },

            get background() {
                return unwrap(this).background;
            },
            set background(newval) {
                unwrap(this).background = String(newval);
            },

            get backgroundAttachment() {
                return unwrap(this).backgroundAttachment;
            },
            set backgroundAttachment(newval) {
                unwrap(this).backgroundAttachment = String(newval);
            },

            get backgroundColor() {
                return unwrap(this).backgroundColor;
            },
            set backgroundColor(newval) {
                unwrap(this).backgroundColor = String(newval);
            },

            get backgroundImage() {
                return unwrap(this).backgroundImage;
            },
            set backgroundImage(newval) {
                unwrap(this).backgroundImage = String(newval);
            },

            get backgroundPosition() {
                return unwrap(this).backgroundPosition;
            },
            set backgroundPosition(newval) {
                unwrap(this).backgroundPosition = String(newval);
            },

            get backgroundRepeat() {
                return unwrap(this).backgroundRepeat;
            },
            set backgroundRepeat(newval) {
                unwrap(this).backgroundRepeat = String(newval);
            },

            get border() {
                return unwrap(this).border;
            },
            set border(newval) {
                unwrap(this).border = String(newval);
            },

            get borderCollapse() {
                return unwrap(this).borderCollapse;
            },
            set borderCollapse(newval) {
                unwrap(this).borderCollapse = String(newval);
            },

            get borderColor() {
                return unwrap(this).borderColor;
            },
            set borderColor(newval) {
                unwrap(this).borderColor = String(newval);
            },

            get borderSpacing() {
                return unwrap(this).borderSpacing;
            },
            set borderSpacing(newval) {
                unwrap(this).borderSpacing = String(newval);
            },

            get borderStyle() {
                return unwrap(this).borderStyle;
            },
            set borderStyle(newval) {
                unwrap(this).borderStyle = String(newval);
            },

            get borderTop() {
                return unwrap(this).borderTop;
            },
            set borderTop(newval) {
                unwrap(this).borderTop = String(newval);
            },

            get borderRight() {
                return unwrap(this).borderRight;
            },
            set borderRight(newval) {
                unwrap(this).borderRight = String(newval);
            },

            get borderBottom() {
                return unwrap(this).borderBottom;
            },
            set borderBottom(newval) {
                unwrap(this).borderBottom = String(newval);
            },

            get borderLeft() {
                return unwrap(this).borderLeft;
            },
            set borderLeft(newval) {
                unwrap(this).borderLeft = String(newval);
            },

            get borderTopColor() {
                return unwrap(this).borderTopColor;
            },
            set borderTopColor(newval) {
                unwrap(this).borderTopColor = String(newval);
            },

            get borderRightColor() {
                return unwrap(this).borderRightColor;
            },
            set borderRightColor(newval) {
                unwrap(this).borderRightColor = String(newval);
            },

            get borderBottomColor() {
                return unwrap(this).borderBottomColor;
            },
            set borderBottomColor(newval) {
                unwrap(this).borderBottomColor = String(newval);
            },

            get borderLeftColor() {
                return unwrap(this).borderLeftColor;
            },
            set borderLeftColor(newval) {
                unwrap(this).borderLeftColor = String(newval);
            },

            get borderTopStyle() {
                return unwrap(this).borderTopStyle;
            },
            set borderTopStyle(newval) {
                unwrap(this).borderTopStyle = String(newval);
            },

            get borderRightStyle() {
                return unwrap(this).borderRightStyle;
            },
            set borderRightStyle(newval) {
                unwrap(this).borderRightStyle = String(newval);
            },

            get borderBottomStyle() {
                return unwrap(this).borderBottomStyle;
            },
            set borderBottomStyle(newval) {
                unwrap(this).borderBottomStyle = String(newval);
            },

            get borderLeftStyle() {
                return unwrap(this).borderLeftStyle;
            },
            set borderLeftStyle(newval) {
                unwrap(this).borderLeftStyle = String(newval);
            },

            get borderTopWidth() {
                return unwrap(this).borderTopWidth;
            },
            set borderTopWidth(newval) {
                unwrap(this).borderTopWidth = String(newval);
            },

            get borderRightWidth() {
                return unwrap(this).borderRightWidth;
            },
            set borderRightWidth(newval) {
                unwrap(this).borderRightWidth = String(newval);
            },

            get borderBottomWidth() {
                return unwrap(this).borderBottomWidth;
            },
            set borderBottomWidth(newval) {
                unwrap(this).borderBottomWidth = String(newval);
            },

            get borderLeftWidth() {
                return unwrap(this).borderLeftWidth;
            },
            set borderLeftWidth(newval) {
                unwrap(this).borderLeftWidth = String(newval);
            },

            get borderWidth() {
                return unwrap(this).borderWidth;
            },
            set borderWidth(newval) {
                unwrap(this).borderWidth = String(newval);
            },

            get bottom() {
                return unwrap(this).bottom;
            },
            set bottom(newval) {
                unwrap(this).bottom = String(newval);
            },

            get captionSide() {
                return unwrap(this).captionSide;
            },
            set captionSide(newval) {
                unwrap(this).captionSide = String(newval);
            },

            get clear() {
                return unwrap(this).clear;
            },
            set clear(newval) {
                unwrap(this).clear = String(newval);
            },

            get clip() {
                return unwrap(this).clip;
            },
            set clip(newval) {
                unwrap(this).clip = String(newval);
            },

            get color() {
                return unwrap(this).color;
            },
            set color(newval) {
                unwrap(this).color = String(newval);
            },

            get content() {
                return unwrap(this).content;
            },
            set content(newval) {
                unwrap(this).content = String(newval);
            },

            get counterIncrement() {
                return unwrap(this).counterIncrement;
            },
            set counterIncrement(newval) {
                unwrap(this).counterIncrement = String(newval);
            },

            get counterReset() {
                return unwrap(this).counterReset;
            },
            set counterReset(newval) {
                unwrap(this).counterReset = String(newval);
            },

            get cursor() {
                return unwrap(this).cursor;
            },
            set cursor(newval) {
                unwrap(this).cursor = String(newval);
            },

            get direction() {
                return unwrap(this).direction;
            },
            set direction(newval) {
                unwrap(this).direction = String(newval);
            },

            get display() {
                return unwrap(this).display;
            },
            set display(newval) {
                unwrap(this).display = String(newval);
            },

            get emptyCells() {
                return unwrap(this).emptyCells;
            },
            set emptyCells(newval) {
                unwrap(this).emptyCells = String(newval);
            },

            get cssFloat() {
                return unwrap(this).cssFloat;
            },
            set cssFloat(newval) {
                unwrap(this).cssFloat = String(newval);
            },

            get font() {
                return unwrap(this).font;
            },
            set font(newval) {
                unwrap(this).font = String(newval);
            },

            get fontFamily() {
                return unwrap(this).fontFamily;
            },
            set fontFamily(newval) {
                unwrap(this).fontFamily = String(newval);
            },

            get fontSize() {
                return unwrap(this).fontSize;
            },
            set fontSize(newval) {
                unwrap(this).fontSize = String(newval);
            },

            get fontSizeAdjust() {
                return unwrap(this).fontSizeAdjust;
            },
            set fontSizeAdjust(newval) {
                unwrap(this).fontSizeAdjust = String(newval);
            },

            get fontStretch() {
                return unwrap(this).fontStretch;
            },
            set fontStretch(newval) {
                unwrap(this).fontStretch = String(newval);
            },

            get fontStyle() {
                return unwrap(this).fontStyle;
            },
            set fontStyle(newval) {
                unwrap(this).fontStyle = String(newval);
            },

            get fontVariant() {
                return unwrap(this).fontVariant;
            },
            set fontVariant(newval) {
                unwrap(this).fontVariant = String(newval);
            },

            get fontWeight() {
                return unwrap(this).fontWeight;
            },
            set fontWeight(newval) {
                unwrap(this).fontWeight = String(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = String(newval);
            },

            get left() {
                return unwrap(this).left;
            },
            set left(newval) {
                unwrap(this).left = String(newval);
            },

            get letterSpacing() {
                return unwrap(this).letterSpacing;
            },
            set letterSpacing(newval) {
                unwrap(this).letterSpacing = String(newval);
            },

            get lineHeight() {
                return unwrap(this).lineHeight;
            },
            set lineHeight(newval) {
                unwrap(this).lineHeight = String(newval);
            },

            get listStyle() {
                return unwrap(this).listStyle;
            },
            set listStyle(newval) {
                unwrap(this).listStyle = String(newval);
            },

            get listStyleImage() {
                return unwrap(this).listStyleImage;
            },
            set listStyleImage(newval) {
                unwrap(this).listStyleImage = String(newval);
            },

            get listStylePosition() {
                return unwrap(this).listStylePosition;
            },
            set listStylePosition(newval) {
                unwrap(this).listStylePosition = String(newval);
            },

            get listStyleType() {
                return unwrap(this).listStyleType;
            },
            set listStyleType(newval) {
                unwrap(this).listStyleType = String(newval);
            },

            get margin() {
                return unwrap(this).margin;
            },
            set margin(newval) {
                unwrap(this).margin = String(newval);
            },

            get marginTop() {
                return unwrap(this).marginTop;
            },
            set marginTop(newval) {
                unwrap(this).marginTop = String(newval);
            },

            get marginRight() {
                return unwrap(this).marginRight;
            },
            set marginRight(newval) {
                unwrap(this).marginRight = String(newval);
            },

            get marginBottom() {
                return unwrap(this).marginBottom;
            },
            set marginBottom(newval) {
                unwrap(this).marginBottom = String(newval);
            },

            get marginLeft() {
                return unwrap(this).marginLeft;
            },
            set marginLeft(newval) {
                unwrap(this).marginLeft = String(newval);
            },

            get markerOffset() {
                return unwrap(this).markerOffset;
            },
            set markerOffset(newval) {
                unwrap(this).markerOffset = String(newval);
            },

            get marks() {
                return unwrap(this).marks;
            },
            set marks(newval) {
                unwrap(this).marks = String(newval);
            },

            get maxHeight() {
                return unwrap(this).maxHeight;
            },
            set maxHeight(newval) {
                unwrap(this).maxHeight = String(newval);
            },

            get maxWidth() {
                return unwrap(this).maxWidth;
            },
            set maxWidth(newval) {
                unwrap(this).maxWidth = String(newval);
            },

            get minHeight() {
                return unwrap(this).minHeight;
            },
            set minHeight(newval) {
                unwrap(this).minHeight = String(newval);
            },

            get minWidth() {
                return unwrap(this).minWidth;
            },
            set minWidth(newval) {
                unwrap(this).minWidth = String(newval);
            },

            get opacity() {
                return unwrap(this).opacity;
            },
            set opacity(newval) {
                unwrap(this).opacity = String(newval);
            },

            get orphans() {
                return unwrap(this).orphans;
            },
            set orphans(newval) {
                unwrap(this).orphans = String(newval);
            },

            get outline() {
                return unwrap(this).outline;
            },
            set outline(newval) {
                unwrap(this).outline = String(newval);
            },

            get outlineColor() {
                return unwrap(this).outlineColor;
            },
            set outlineColor(newval) {
                unwrap(this).outlineColor = String(newval);
            },

            get outlineStyle() {
                return unwrap(this).outlineStyle;
            },
            set outlineStyle(newval) {
                unwrap(this).outlineStyle = String(newval);
            },

            get outlineWidth() {
                return unwrap(this).outlineWidth;
            },
            set outlineWidth(newval) {
                unwrap(this).outlineWidth = String(newval);
            },

            get overflow() {
                return unwrap(this).overflow;
            },
            set overflow(newval) {
                unwrap(this).overflow = String(newval);
            },

            get padding() {
                return unwrap(this).padding;
            },
            set padding(newval) {
                unwrap(this).padding = String(newval);
            },

            get paddingTop() {
                return unwrap(this).paddingTop;
            },
            set paddingTop(newval) {
                unwrap(this).paddingTop = String(newval);
            },

            get paddingRight() {
                return unwrap(this).paddingRight;
            },
            set paddingRight(newval) {
                unwrap(this).paddingRight = String(newval);
            },

            get paddingBottom() {
                return unwrap(this).paddingBottom;
            },
            set paddingBottom(newval) {
                unwrap(this).paddingBottom = String(newval);
            },

            get paddingLeft() {
                return unwrap(this).paddingLeft;
            },
            set paddingLeft(newval) {
                unwrap(this).paddingLeft = String(newval);
            },

            get page() {
                return unwrap(this).page;
            },
            set page(newval) {
                unwrap(this).page = String(newval);
            },

            get pageBreakAfter() {
                return unwrap(this).pageBreakAfter;
            },
            set pageBreakAfter(newval) {
                unwrap(this).pageBreakAfter = String(newval);
            },

            get pageBreakBefore() {
                return unwrap(this).pageBreakBefore;
            },
            set pageBreakBefore(newval) {
                unwrap(this).pageBreakBefore = String(newval);
            },

            get pageBreakInside() {
                return unwrap(this).pageBreakInside;
            },
            set pageBreakInside(newval) {
                unwrap(this).pageBreakInside = String(newval);
            },

            get position() {
                return unwrap(this).position;
            },
            set position(newval) {
                unwrap(this).position = String(newval);
            },

            get quotes() {
                return unwrap(this).quotes;
            },
            set quotes(newval) {
                unwrap(this).quotes = String(newval);
            },

            get right() {
                return unwrap(this).right;
            },
            set right(newval) {
                unwrap(this).right = String(newval);
            },

            get size() {
                return unwrap(this).size;
            },
            set size(newval) {
                unwrap(this).size = String(newval);
            },

            get tableLayout() {
                return unwrap(this).tableLayout;
            },
            set tableLayout(newval) {
                unwrap(this).tableLayout = String(newval);
            },

            get textAlign() {
                return unwrap(this).textAlign;
            },
            set textAlign(newval) {
                unwrap(this).textAlign = String(newval);
            },

            get textDecoration() {
                return unwrap(this).textDecoration;
            },
            set textDecoration(newval) {
                unwrap(this).textDecoration = String(newval);
            },

            get textIndent() {
                return unwrap(this).textIndent;
            },
            set textIndent(newval) {
                unwrap(this).textIndent = String(newval);
            },

            get textShadow() {
                return unwrap(this).textShadow;
            },
            set textShadow(newval) {
                unwrap(this).textShadow = String(newval);
            },

            get textTransform() {
                return unwrap(this).textTransform;
            },
            set textTransform(newval) {
                unwrap(this).textTransform = String(newval);
            },

            get top() {
                return unwrap(this).top;
            },
            set top(newval) {
                unwrap(this).top = String(newval);
            },

            get unicodeBidi() {
                return unwrap(this).unicodeBidi;
            },
            set unicodeBidi(newval) {
                unwrap(this).unicodeBidi = String(newval);
            },

            get verticalAlign() {
                return unwrap(this).verticalAlign;
            },
            set verticalAlign(newval) {
                unwrap(this).verticalAlign = String(newval);
            },

            get visibility() {
                return unwrap(this).visibility;
            },
            set visibility(newval) {
                unwrap(this).visibility = String(newval);
            },

            get whiteSpace() {
                return unwrap(this).whiteSpace;
            },
            set whiteSpace(newval) {
                unwrap(this).whiteSpace = String(newval);
            },

            get widows() {
                return unwrap(this).widows;
            },
            set widows(newval) {
                unwrap(this).widows = String(newval);
            },

            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = String(newval);
            },

            get wordSpacing() {
                return unwrap(this).wordSpacing;
            },
            set wordSpacing(newval) {
                unwrap(this).wordSpacing = String(newval);
            },

            get zIndex() {
                return unwrap(this).zIndex;
            },
            set zIndex(newval) {
                unwrap(this).zIndex = String(newval);
            },

        },
    });
});

//
// Interface HTMLElement
//

defineLazyProperty(global, "HTMLElement", function() {
    return idl.HTMLElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLElement", function() {
    return new IDLInterface({
        name: "HTMLElement",
        superclass: idl.Element,
        members: {
            get innerHTML() {
                return unwrap(this).innerHTML;
            },
            set innerHTML(newval) {
                unwrap(this).innerHTML = String(newval);
            },

            get outerHTML() {
                return unwrap(this).outerHTML;
            },
            set outerHTML(newval) {
                unwrap(this).outerHTML = String(newval);
            },

            insertAdjacentHTML: function insertAdjacentHTML(
                                    position,
                                    text)
            {
                unwrap(this).insertAdjacentHTML(
                    String(position),
                    String(text));
            },

            get id() {
                return unwrap(this).id;
            },
            set id(newval) {
                unwrap(this).id = String(newval);
            },

            get title() {
                return unwrap(this).title;
            },
            set title(newval) {
                unwrap(this).title = String(newval);
            },

            get lang() {
                return unwrap(this).lang;
            },
            set lang(newval) {
                unwrap(this).lang = String(newval);
            },

            get dir() {
                return unwrap(this).dir;
            },
            set dir(newval) {
                unwrap(this).dir = String(newval);
            },

            get className() {
                return unwrap(this).className;
            },
            set className(newval) {
                unwrap(this).className = String(newval);
            },

            get classList() {
                return wrap(unwrap(this).classList);
            },

            get dataset() {
                return wrap(unwrap(this).dataset);
            },

            get itemScope() {
                return unwrap(this).itemScope;
            },
            set itemScope(newval) {
                unwrap(this).itemScope = Boolean(newval);
            },

            get itemType() {
                return unwrap(this).itemType;
            },
            set itemType(newval) {
                unwrap(this).itemType = String(newval);
            },

            get itemId() {
                return unwrap(this).itemId;
            },
            set itemId(newval) {
                unwrap(this).itemId = String(newval);
            },

            get itemRef() {
                return wrap(unwrap(this).itemRef);
            },

            get itemProp() {
                return wrap(unwrap(this).itemProp);
            },

            get properties() {
                return wrap(unwrap(this).properties);
            },

            get itemValue() {
                return unwrap(this).itemValue;
            },
            set itemValue(newval) {
                unwrap(this).itemValue = newval;
            },

            get hidden() {
                return unwrap(this).hidden;
            },
            set hidden(newval) {
                unwrap(this).hidden = Boolean(newval);
            },

            click: function click() {
                unwrap(this).click();
            },

            get tabIndex() {
                return unwrap(this).tabIndex;
            },
            set tabIndex(newval) {
                unwrap(this).tabIndex = toLong(newval);
            },

            focus: function focus() {
                unwrap(this).focus();
            },

            blur: function blur() {
                unwrap(this).blur();
            },

            get accessKey() {
                return unwrap(this).accessKey;
            },
            set accessKey(newval) {
                unwrap(this).accessKey = String(newval);
            },

            get accessKeyLabel() {
                return unwrap(this).accessKeyLabel;
            },

            get draggable() {
                return unwrap(this).draggable;
            },
            set draggable(newval) {
                unwrap(this).draggable = Boolean(newval);
            },

            get dropzone() {
                return wrap(unwrap(this).dropzone);
            },

            get contentEditable() {
                return unwrap(this).contentEditable;
            },
            set contentEditable(newval) {
                unwrap(this).contentEditable = String(newval);
            },

            get isContentEditable() {
                return unwrap(this).isContentEditable;
            },

            get contextMenu() {
                return wrap(unwrap(this).contextMenu);
            },
            set contextMenu(newval) {
                unwrap(this).contextMenu = unwrapOrNull(newval);
            },

            get spellcheck() {
                return unwrap(this).spellcheck;
            },
            set spellcheck(newval) {
                unwrap(this).spellcheck = Boolean(newval);
            },

            get commandType() {
                return unwrap(this).commandType;
            },

            get commandLabel() {
                return unwrap(this).commandLabel;
            },

            get commandIcon() {
                return unwrap(this).commandIcon;
            },

            get commandHidden() {
                return unwrap(this).commandHidden;
            },

            get commandDisabled() {
                return unwrap(this).commandDisabled;
            },

            get commandChecked() {
                return unwrap(this).commandChecked;
            },

            get style() {
                return wrap(unwrap(this).style);
            },

            get onabort() {
                return unwrap(this).onabort;
            },
            set onabort(newval) {
                unwrap(this).onabort = toCallbackOrNull(newval);
            },

            get onblur() {
                return unwrap(this).onblur;
            },
            set onblur(newval) {
                unwrap(this).onblur = toCallbackOrNull(newval);
            },

            get oncanplay() {
                return unwrap(this).oncanplay;
            },
            set oncanplay(newval) {
                unwrap(this).oncanplay = toCallbackOrNull(newval);
            },

            get oncanplaythrough() {
                return unwrap(this).oncanplaythrough;
            },
            set oncanplaythrough(newval) {
                unwrap(this).oncanplaythrough = toCallbackOrNull(newval);
            },

            get onchange() {
                return unwrap(this).onchange;
            },
            set onchange(newval) {
                unwrap(this).onchange = toCallbackOrNull(newval);
            },

            get onclick() {
                return unwrap(this).onclick;
            },
            set onclick(newval) {
                unwrap(this).onclick = toCallbackOrNull(newval);
            },

            get oncontextmenu() {
                return unwrap(this).oncontextmenu;
            },
            set oncontextmenu(newval) {
                unwrap(this).oncontextmenu = toCallbackOrNull(newval);
            },

            get oncuechange() {
                return unwrap(this).oncuechange;
            },
            set oncuechange(newval) {
                unwrap(this).oncuechange = toCallbackOrNull(newval);
            },

            get ondblclick() {
                return unwrap(this).ondblclick;
            },
            set ondblclick(newval) {
                unwrap(this).ondblclick = toCallbackOrNull(newval);
            },

            get ondrag() {
                return unwrap(this).ondrag;
            },
            set ondrag(newval) {
                unwrap(this).ondrag = toCallbackOrNull(newval);
            },

            get ondragend() {
                return unwrap(this).ondragend;
            },
            set ondragend(newval) {
                unwrap(this).ondragend = toCallbackOrNull(newval);
            },

            get ondragenter() {
                return unwrap(this).ondragenter;
            },
            set ondragenter(newval) {
                unwrap(this).ondragenter = toCallbackOrNull(newval);
            },

            get ondragleave() {
                return unwrap(this).ondragleave;
            },
            set ondragleave(newval) {
                unwrap(this).ondragleave = toCallbackOrNull(newval);
            },

            get ondragover() {
                return unwrap(this).ondragover;
            },
            set ondragover(newval) {
                unwrap(this).ondragover = toCallbackOrNull(newval);
            },

            get ondragstart() {
                return unwrap(this).ondragstart;
            },
            set ondragstart(newval) {
                unwrap(this).ondragstart = toCallbackOrNull(newval);
            },

            get ondrop() {
                return unwrap(this).ondrop;
            },
            set ondrop(newval) {
                unwrap(this).ondrop = toCallbackOrNull(newval);
            },

            get ondurationchange() {
                return unwrap(this).ondurationchange;
            },
            set ondurationchange(newval) {
                unwrap(this).ondurationchange = toCallbackOrNull(newval);
            },

            get onemptied() {
                return unwrap(this).onemptied;
            },
            set onemptied(newval) {
                unwrap(this).onemptied = toCallbackOrNull(newval);
            },

            get onended() {
                return unwrap(this).onended;
            },
            set onended(newval) {
                unwrap(this).onended = toCallbackOrNull(newval);
            },

            get onerror() {
                return unwrap(this).onerror;
            },
            set onerror(newval) {
                unwrap(this).onerror = toCallbackOrNull(newval);
            },

            get onfocus() {
                return unwrap(this).onfocus;
            },
            set onfocus(newval) {
                unwrap(this).onfocus = toCallbackOrNull(newval);
            },

            get oninput() {
                return unwrap(this).oninput;
            },
            set oninput(newval) {
                unwrap(this).oninput = toCallbackOrNull(newval);
            },

            get oninvalid() {
                return unwrap(this).oninvalid;
            },
            set oninvalid(newval) {
                unwrap(this).oninvalid = toCallbackOrNull(newval);
            },

            get onkeydown() {
                return unwrap(this).onkeydown;
            },
            set onkeydown(newval) {
                unwrap(this).onkeydown = toCallbackOrNull(newval);
            },

            get onkeypress() {
                return unwrap(this).onkeypress;
            },
            set onkeypress(newval) {
                unwrap(this).onkeypress = toCallbackOrNull(newval);
            },

            get onkeyup() {
                return unwrap(this).onkeyup;
            },
            set onkeyup(newval) {
                unwrap(this).onkeyup = toCallbackOrNull(newval);
            },

            get onload() {
                return unwrap(this).onload;
            },
            set onload(newval) {
                unwrap(this).onload = toCallbackOrNull(newval);
            },

            get onloadeddata() {
                return unwrap(this).onloadeddata;
            },
            set onloadeddata(newval) {
                unwrap(this).onloadeddata = toCallbackOrNull(newval);
            },

            get onloadedmetadata() {
                return unwrap(this).onloadedmetadata;
            },
            set onloadedmetadata(newval) {
                unwrap(this).onloadedmetadata = toCallbackOrNull(newval);
            },

            get onloadstart() {
                return unwrap(this).onloadstart;
            },
            set onloadstart(newval) {
                unwrap(this).onloadstart = toCallbackOrNull(newval);
            },

            get onmousedown() {
                return unwrap(this).onmousedown;
            },
            set onmousedown(newval) {
                unwrap(this).onmousedown = toCallbackOrNull(newval);
            },

            get onmousemove() {
                return unwrap(this).onmousemove;
            },
            set onmousemove(newval) {
                unwrap(this).onmousemove = toCallbackOrNull(newval);
            },

            get onmouseout() {
                return unwrap(this).onmouseout;
            },
            set onmouseout(newval) {
                unwrap(this).onmouseout = toCallbackOrNull(newval);
            },

            get onmouseover() {
                return unwrap(this).onmouseover;
            },
            set onmouseover(newval) {
                unwrap(this).onmouseover = toCallbackOrNull(newval);
            },

            get onmouseup() {
                return unwrap(this).onmouseup;
            },
            set onmouseup(newval) {
                unwrap(this).onmouseup = toCallbackOrNull(newval);
            },

            get onmousewheel() {
                return unwrap(this).onmousewheel;
            },
            set onmousewheel(newval) {
                unwrap(this).onmousewheel = toCallbackOrNull(newval);
            },

            get onpause() {
                return unwrap(this).onpause;
            },
            set onpause(newval) {
                unwrap(this).onpause = toCallbackOrNull(newval);
            },

            get onplay() {
                return unwrap(this).onplay;
            },
            set onplay(newval) {
                unwrap(this).onplay = toCallbackOrNull(newval);
            },

            get onplaying() {
                return unwrap(this).onplaying;
            },
            set onplaying(newval) {
                unwrap(this).onplaying = toCallbackOrNull(newval);
            },

            get onprogress() {
                return unwrap(this).onprogress;
            },
            set onprogress(newval) {
                unwrap(this).onprogress = toCallbackOrNull(newval);
            },

            get onratechange() {
                return unwrap(this).onratechange;
            },
            set onratechange(newval) {
                unwrap(this).onratechange = toCallbackOrNull(newval);
            },

            get onreadystatechange() {
                return unwrap(this).onreadystatechange;
            },
            set onreadystatechange(newval) {
                unwrap(this).onreadystatechange = toCallbackOrNull(newval);
            },

            get onreset() {
                return unwrap(this).onreset;
            },
            set onreset(newval) {
                unwrap(this).onreset = toCallbackOrNull(newval);
            },

            get onscroll() {
                return unwrap(this).onscroll;
            },
            set onscroll(newval) {
                unwrap(this).onscroll = toCallbackOrNull(newval);
            },

            get onseeked() {
                return unwrap(this).onseeked;
            },
            set onseeked(newval) {
                unwrap(this).onseeked = toCallbackOrNull(newval);
            },

            get onseeking() {
                return unwrap(this).onseeking;
            },
            set onseeking(newval) {
                unwrap(this).onseeking = toCallbackOrNull(newval);
            },

            get onselect() {
                return unwrap(this).onselect;
            },
            set onselect(newval) {
                unwrap(this).onselect = toCallbackOrNull(newval);
            },

            get onshow() {
                return unwrap(this).onshow;
            },
            set onshow(newval) {
                unwrap(this).onshow = toCallbackOrNull(newval);
            },

            get onstalled() {
                return unwrap(this).onstalled;
            },
            set onstalled(newval) {
                unwrap(this).onstalled = toCallbackOrNull(newval);
            },

            get onsubmit() {
                return unwrap(this).onsubmit;
            },
            set onsubmit(newval) {
                unwrap(this).onsubmit = toCallbackOrNull(newval);
            },

            get onsuspend() {
                return unwrap(this).onsuspend;
            },
            set onsuspend(newval) {
                unwrap(this).onsuspend = toCallbackOrNull(newval);
            },

            get ontimeupdate() {
                return unwrap(this).ontimeupdate;
            },
            set ontimeupdate(newval) {
                unwrap(this).ontimeupdate = toCallbackOrNull(newval);
            },

            get onvolumechange() {
                return unwrap(this).onvolumechange;
            },
            set onvolumechange(newval) {
                unwrap(this).onvolumechange = toCallbackOrNull(newval);
            },

            get onwaiting() {
                return unwrap(this).onwaiting;
            },
            set onwaiting(newval) {
                unwrap(this).onwaiting = toCallbackOrNull(newval);
            },

        },
    });
});

//
// Interface HTMLUnknownElement
//

defineLazyProperty(global, "HTMLUnknownElement", function() {
    return idl.HTMLUnknownElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLUnknownElement", function() {
    return new IDLInterface({
        name: "HTMLUnknownElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLHtmlElement
//

defineLazyProperty(global, "HTMLHtmlElement", function() {
    return idl.HTMLHtmlElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLHtmlElement", function() {
    return new IDLInterface({
        name: "HTMLHtmlElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLHeadElement
//

defineLazyProperty(global, "HTMLHeadElement", function() {
    return idl.HTMLHeadElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLHeadElement", function() {
    return new IDLInterface({
        name: "HTMLHeadElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLTitleElement
//

defineLazyProperty(global, "HTMLTitleElement", function() {
    return idl.HTMLTitleElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTitleElement", function() {
    return new IDLInterface({
        name: "HTMLTitleElement",
        superclass: idl.HTMLElement,
        members: {
            get text() {
                return unwrap(this).text;
            },
            set text(newval) {
                unwrap(this).text = String(newval);
            },

        },
    });
});

//
// Interface HTMLBaseElement
//

defineLazyProperty(global, "HTMLBaseElement", function() {
    return idl.HTMLBaseElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLBaseElement", function() {
    return new IDLInterface({
        name: "HTMLBaseElement",
        superclass: idl.HTMLElement,
        members: {
            get href() {
                return unwrap(this).href;
            },
            set href(newval) {
                unwrap(this).href = String(newval);
            },

            get target() {
                return unwrap(this).target;
            },
            set target(newval) {
                unwrap(this).target = String(newval);
            },

        },
    });
});

//
// Interface HTMLLinkElement
//

defineLazyProperty(global, "HTMLLinkElement", function() {
    return idl.HTMLLinkElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLLinkElement", function() {
    return new IDLInterface({
        name: "HTMLLinkElement",
        superclass: idl.HTMLElement,
        members: {
            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get href() {
                return unwrap(this).href;
            },
            set href(newval) {
                unwrap(this).href = String(newval);
            },

            get rel() {
                return unwrap(this).rel;
            },
            set rel(newval) {
                unwrap(this).rel = String(newval);
            },

            get relList() {
                return wrap(unwrap(this).relList);
            },

            get media() {
                return unwrap(this).media;
            },
            set media(newval) {
                unwrap(this).media = String(newval);
            },

            get hreflang() {
                return unwrap(this).hreflang;
            },
            set hreflang(newval) {
                unwrap(this).hreflang = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get sizes() {
                return wrap(unwrap(this).sizes);
            },

        },
    });
});

//
// Interface HTMLMetaElement
//

defineLazyProperty(global, "HTMLMetaElement", function() {
    return idl.HTMLMetaElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLMetaElement", function() {
    return new IDLInterface({
        name: "HTMLMetaElement",
        superclass: idl.HTMLElement,
        members: {
            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get httpEquiv() {
                return unwrap(this).httpEquiv;
            },
            set httpEquiv(newval) {
                unwrap(this).httpEquiv = String(newval);
            },

            get content() {
                return unwrap(this).content;
            },
            set content(newval) {
                unwrap(this).content = String(newval);
            },

        },
    });
});

//
// Interface HTMLStyleElement
//

defineLazyProperty(global, "HTMLStyleElement", function() {
    return idl.HTMLStyleElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLStyleElement", function() {
    return new IDLInterface({
        name: "HTMLStyleElement",
        superclass: idl.HTMLElement,
        members: {
            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get media() {
                return unwrap(this).media;
            },
            set media(newval) {
                unwrap(this).media = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get scoped() {
                return unwrap(this).scoped;
            },
            set scoped(newval) {
                unwrap(this).scoped = Boolean(newval);
            },

        },
    });
});

//
// Interface HTMLScriptElement
//

defineLazyProperty(global, "HTMLScriptElement", function() {
    return idl.HTMLScriptElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLScriptElement", function() {
    return new IDLInterface({
        name: "HTMLScriptElement",
        superclass: idl.HTMLElement,
        members: {
            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get async() {
                return unwrap(this).async;
            },
            set async(newval) {
                unwrap(this).async = Boolean(newval);
            },

            get defer() {
                return unwrap(this).defer;
            },
            set defer(newval) {
                unwrap(this).defer = Boolean(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get charset() {
                return unwrap(this).charset;
            },
            set charset(newval) {
                unwrap(this).charset = String(newval);
            },

            get text() {
                return unwrap(this).text;
            },
            set text(newval) {
                unwrap(this).text = String(newval);
            },

        },
    });
});

//
// Interface HTMLBodyElement
//

defineLazyProperty(global, "HTMLBodyElement", function() {
    return idl.HTMLBodyElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLBodyElement", function() {
    return new IDLInterface({
        name: "HTMLBodyElement",
        superclass: idl.HTMLElement,
        members: {
            get onafterprint() {
                return unwrap(this).onafterprint;
            },
            set onafterprint(newval) {
                unwrap(this).onafterprint = toCallbackOrNull(newval);
            },

            get onbeforeprint() {
                return unwrap(this).onbeforeprint;
            },
            set onbeforeprint(newval) {
                unwrap(this).onbeforeprint = toCallbackOrNull(newval);
            },

            get onbeforeunload() {
                return unwrap(this).onbeforeunload;
            },
            set onbeforeunload(newval) {
                unwrap(this).onbeforeunload = toCallbackOrNull(newval);
            },

            get onblur() {
                return unwrap(this).onblur;
            },
            set onblur(newval) {
                unwrap(this).onblur = toCallbackOrNull(newval);
            },

            get onerror() {
                return unwrap(this).onerror;
            },
            set onerror(newval) {
                unwrap(this).onerror = toCallbackOrNull(newval);
            },

            get onfocus() {
                return unwrap(this).onfocus;
            },
            set onfocus(newval) {
                unwrap(this).onfocus = toCallbackOrNull(newval);
            },

            get onhashchange() {
                return unwrap(this).onhashchange;
            },
            set onhashchange(newval) {
                unwrap(this).onhashchange = toCallbackOrNull(newval);
            },

            get onload() {
                return unwrap(this).onload;
            },
            set onload(newval) {
                unwrap(this).onload = toCallbackOrNull(newval);
            },

            get onmessage() {
                return unwrap(this).onmessage;
            },
            set onmessage(newval) {
                unwrap(this).onmessage = toCallbackOrNull(newval);
            },

            get onoffline() {
                return unwrap(this).onoffline;
            },
            set onoffline(newval) {
                unwrap(this).onoffline = toCallbackOrNull(newval);
            },

            get ononline() {
                return unwrap(this).ononline;
            },
            set ononline(newval) {
                unwrap(this).ononline = toCallbackOrNull(newval);
            },

            get onpopstate() {
                return unwrap(this).onpopstate;
            },
            set onpopstate(newval) {
                unwrap(this).onpopstate = toCallbackOrNull(newval);
            },

            get onpagehide() {
                return unwrap(this).onpagehide;
            },
            set onpagehide(newval) {
                unwrap(this).onpagehide = toCallbackOrNull(newval);
            },

            get onpageshow() {
                return unwrap(this).onpageshow;
            },
            set onpageshow(newval) {
                unwrap(this).onpageshow = toCallbackOrNull(newval);
            },

            get onredo() {
                return unwrap(this).onredo;
            },
            set onredo(newval) {
                unwrap(this).onredo = toCallbackOrNull(newval);
            },

            get onresize() {
                return unwrap(this).onresize;
            },
            set onresize(newval) {
                unwrap(this).onresize = toCallbackOrNull(newval);
            },

            get onscroll() {
                return unwrap(this).onscroll;
            },
            set onscroll(newval) {
                unwrap(this).onscroll = toCallbackOrNull(newval);
            },

            get onstorage() {
                return unwrap(this).onstorage;
            },
            set onstorage(newval) {
                unwrap(this).onstorage = toCallbackOrNull(newval);
            },

            get onundo() {
                return unwrap(this).onundo;
            },
            set onundo(newval) {
                unwrap(this).onundo = toCallbackOrNull(newval);
            },

            get onunload() {
                return unwrap(this).onunload;
            },
            set onunload(newval) {
                unwrap(this).onunload = toCallbackOrNull(newval);
            },

        },
    });
});

//
// Interface HTMLHeadingElement
//

defineLazyProperty(global, "HTMLHeadingElement", function() {
    return idl.HTMLHeadingElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLHeadingElement", function() {
    return new IDLInterface({
        name: "HTMLHeadingElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLParagraphElement
//

defineLazyProperty(global, "HTMLParagraphElement", function() {
    return idl.HTMLParagraphElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLParagraphElement", function() {
    return new IDLInterface({
        name: "HTMLParagraphElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLHRElement
//

defineLazyProperty(global, "HTMLHRElement", function() {
    return idl.HTMLHRElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLHRElement", function() {
    return new IDLInterface({
        name: "HTMLHRElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLPreElement
//

defineLazyProperty(global, "HTMLPreElement", function() {
    return idl.HTMLPreElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLPreElement", function() {
    return new IDLInterface({
        name: "HTMLPreElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLQuoteElement
//

defineLazyProperty(global, "HTMLQuoteElement", function() {
    return idl.HTMLQuoteElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLQuoteElement", function() {
    return new IDLInterface({
        name: "HTMLQuoteElement",
        superclass: idl.HTMLElement,
        members: {
            get cite() {
                return unwrap(this).cite;
            },
            set cite(newval) {
                unwrap(this).cite = String(newval);
            },

        },
    });
});

//
// Interface HTMLOListElement
//

defineLazyProperty(global, "HTMLOListElement", function() {
    return idl.HTMLOListElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLOListElement", function() {
    return new IDLInterface({
        name: "HTMLOListElement",
        superclass: idl.HTMLElement,
        members: {
            get reversed() {
                return unwrap(this).reversed;
            },
            set reversed(newval) {
                unwrap(this).reversed = Boolean(newval);
            },

            get start() {
                return unwrap(this).start;
            },
            set start(newval) {
                unwrap(this).start = toLong(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

        },
    });
});

//
// Interface HTMLUListElement
//

defineLazyProperty(global, "HTMLUListElement", function() {
    return idl.HTMLUListElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLUListElement", function() {
    return new IDLInterface({
        name: "HTMLUListElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLLIElement
//

defineLazyProperty(global, "HTMLLIElement", function() {
    return idl.HTMLLIElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLLIElement", function() {
    return new IDLInterface({
        name: "HTMLLIElement",
        superclass: idl.HTMLElement,
        members: {
            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = toLong(newval);
            },

        },
    });
});

//
// Interface HTMLDListElement
//

defineLazyProperty(global, "HTMLDListElement", function() {
    return idl.HTMLDListElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLDListElement", function() {
    return new IDLInterface({
        name: "HTMLDListElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLDivElement
//

defineLazyProperty(global, "HTMLDivElement", function() {
    return idl.HTMLDivElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLDivElement", function() {
    return new IDLInterface({
        name: "HTMLDivElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLAnchorElement
//

defineLazyProperty(global, "HTMLAnchorElement", function() {
    return idl.HTMLAnchorElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLAnchorElement", function() {
    return new IDLInterface({
        name: "HTMLAnchorElement",
        superclass: idl.HTMLElement,
        members: {
            get href() {
                return unwrap(this).href;
            },
            set href(newval) {
                unwrap(this).href = String(newval);
            },

            get target() {
                return unwrap(this).target;
            },
            set target(newval) {
                unwrap(this).target = String(newval);
            },

            get download() {
                return unwrap(this).download;
            },
            set download(newval) {
                unwrap(this).download = String(newval);
            },

            get ping() {
                return unwrap(this).ping;
            },
            set ping(newval) {
                unwrap(this).ping = String(newval);
            },

            get rel() {
                return unwrap(this).rel;
            },
            set rel(newval) {
                unwrap(this).rel = String(newval);
            },

            get relList() {
                return wrap(unwrap(this).relList);
            },

            get media() {
                return unwrap(this).media;
            },
            set media(newval) {
                unwrap(this).media = String(newval);
            },

            get hreflang() {
                return unwrap(this).hreflang;
            },
            set hreflang(newval) {
                unwrap(this).hreflang = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get text() {
                return unwrap(this).text;
            },
            set text(newval) {
                unwrap(this).text = String(newval);
            },

            get protocol() {
                return unwrap(this).protocol;
            },
            set protocol(newval) {
                unwrap(this).protocol = String(newval);
            },

            get host() {
                return unwrap(this).host;
            },
            set host(newval) {
                unwrap(this).host = String(newval);
            },

            get hostname() {
                return unwrap(this).hostname;
            },
            set hostname(newval) {
                unwrap(this).hostname = String(newval);
            },

            get port() {
                return unwrap(this).port;
            },
            set port(newval) {
                unwrap(this).port = String(newval);
            },

            get pathname() {
                return unwrap(this).pathname;
            },
            set pathname(newval) {
                unwrap(this).pathname = String(newval);
            },

            get search() {
                return unwrap(this).search;
            },
            set search(newval) {
                unwrap(this).search = String(newval);
            },

            get hash() {
                return unwrap(this).hash;
            },
            set hash(newval) {
                unwrap(this).hash = String(newval);
            },

        },
    });
});

//
// Interface HTMLCanvasElement
//

defineLazyProperty(global, "HTMLCanvasElement", function() {
    return idl.HTMLCanvasElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLCanvasElement", function() {
    return new IDLInterface({
        name: "HTMLCanvasElement",
        superclass: idl.HTMLElement,
        members: {
            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = toULong(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = toULong(newval);
            },

            get clientWidth() {
                return unwrap(this).clientWidth;
            },

            get clientHeight() {
                return unwrap(this).clientHeight;
            },

            getContext: function getContext(type) {
                return unwrap(this).getContext(String(type));
            },

        },
    });
});

//
// Interface HTMLHoloCanvasElementExp
//

defineLazyProperty(global, "HTMLHoloCanvasElementExp", function() {
    return idl.HTMLHoloCanvasElementExp.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLHoloCanvasElementExp", function() {
    return new IDLInterface({
        name: "HTMLHoloCanvasElementExp",
        superclass: idl.HTMLElement,
        members: {
            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = toULong(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = toULong(newval);
            },

            get clientWidth() {
                return unwrap(this).clientWidth;
            },

            get clientHeight() {
                return unwrap(this).clientHeight;
            },

            getContext: function getContext(type) {
                return unwrap(this).getContext(String(type));
            },

            getBoundingClientRect: function getBoundingClientRect() {
                return wrap(unwrap(this).getBoundingClientRect());
            },

        },
    });
});

//
// Interface HTMLTimeElement
//

defineLazyProperty(global, "HTMLTimeElement", function() {
    return idl.HTMLTimeElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTimeElement", function() {
    return new IDLInterface({
        name: "HTMLTimeElement",
        superclass: idl.HTMLElement,
        members: {
            get dateTime() {
                return unwrap(this).dateTime;
            },
            set dateTime(newval) {
                unwrap(this).dateTime = String(newval);
            },

            get pubDate() {
                return unwrap(this).pubDate;
            },
            set pubDate(newval) {
                unwrap(this).pubDate = Boolean(newval);
            },

            get valueAsDate() {
                return wrap(unwrap(this).valueAsDate);
            },

        },
    });
});

//
// Interface HTMLSpanElement
//

defineLazyProperty(global, "HTMLSpanElement", function() {
    return idl.HTMLSpanElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLSpanElement", function() {
    return new IDLInterface({
        name: "HTMLSpanElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLBRElement
//

defineLazyProperty(global, "HTMLBRElement", function() {
    return idl.HTMLBRElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLBRElement", function() {
    return new IDLInterface({
        name: "HTMLBRElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLModElement
//

defineLazyProperty(global, "HTMLModElement", function() {
    return idl.HTMLModElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLModElement", function() {
    return new IDLInterface({
        name: "HTMLModElement",
        superclass: idl.HTMLElement,
        members: {
            get cite() {
                return unwrap(this).cite;
            },
            set cite(newval) {
                unwrap(this).cite = String(newval);
            },

            get dateTime() {
                return unwrap(this).dateTime;
            },
            set dateTime(newval) {
                unwrap(this).dateTime = String(newval);
            },

        },
    });
});

//
// Interface HTMLImageElement
//

defineLazyProperty(global, "HTMLImageElement", function() {
    return idl.HTMLImageElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLImageElement", function() {
    return new IDLInterface({
        name: "HTMLImageElement",
        superclass: idl.HTMLElement,
        members: {
            get alt() {
                return unwrap(this).alt;
            },
            set alt(newval) {
                unwrap(this).alt = String(newval);
            },

            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get crossOrigin() {
                return unwrap(this).crossOrigin;
            },
            set crossOrigin(newval) {
                unwrap(this).crossOrigin = String(newval);
            },

            get useMap() {
                return unwrap(this).useMap;
            },
            set useMap(newval) {
                unwrap(this).useMap = String(newval);
            },

            get isMap() {
                return unwrap(this).isMap;
            },
            set isMap(newval) {
                unwrap(this).isMap = Boolean(newval);
            },

            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = toULong(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = toULong(newval);
            },

            get naturalWidth() {
                return unwrap(this).naturalWidth;
            },

            get naturalHeight() {
                return unwrap(this).naturalHeight;
            },

            get complete() {
                return unwrap(this).complete;
            },

            getData: function getData() {
                return unwrap(this).getData();
            },

        },
    });
});

//
// Interface HTMLIFrameElement
//

defineLazyProperty(global, "HTMLIFrameElement", function() {
    return idl.HTMLIFrameElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLIFrameElement", function() {
    return new IDLInterface({
        name: "HTMLIFrameElement",
        superclass: idl.HTMLElement,
        members: {
            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get srcdoc() {
                return unwrap(this).srcdoc;
            },
            set srcdoc(newval) {
                unwrap(this).srcdoc = String(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get sandbox() {
                return wrap(unwrap(this).sandbox);
            },

            get seamless() {
                return unwrap(this).seamless;
            },
            set seamless(newval) {
                unwrap(this).seamless = Boolean(newval);
            },

            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = String(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = String(newval);
            },

            get contentDocument() {
                return wrap(unwrap(this).contentDocument);
            },

            get contentWindow() {
                return wrap(unwrap(this).contentWindow);
            },

        },
    });
});

//
// Interface HTMLEmbedElement
//

defineLazyProperty(global, "HTMLEmbedElement", function() {
    return idl.HTMLEmbedElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLEmbedElement", function() {
    return new IDLInterface({
        name: "HTMLEmbedElement",
        superclass: idl.HTMLElement,
        members: {
            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = String(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = String(newval);
            },

        },
    });
});

//
// Interface HTMLObjectElement
//

defineLazyProperty(global, "HTMLObjectElement", function() {
    return idl.HTMLObjectElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLObjectElement", function() {
    return new IDLInterface({
        name: "HTMLObjectElement",
        superclass: idl.HTMLElement,
        members: {
            get data() {
                return unwrap(this).data;
            },
            set data(newval) {
                unwrap(this).data = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get typeMustMatch() {
                return unwrap(this).typeMustMatch;
            },
            set typeMustMatch(newval) {
                unwrap(this).typeMustMatch = Boolean(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get useMap() {
                return unwrap(this).useMap;
            },
            set useMap(newval) {
                unwrap(this).useMap = String(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = String(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = String(newval);
            },

            get contentDocument() {
                return wrap(unwrap(this).contentDocument);
            },

            get contentWindow() {
                return wrap(unwrap(this).contentWindow);
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

        },
    });
});

//
// Interface HTMLParamElement
//

defineLazyProperty(global, "HTMLParamElement", function() {
    return idl.HTMLParamElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLParamElement", function() {
    return new IDLInterface({
        name: "HTMLParamElement",
        superclass: idl.HTMLElement,
        members: {
            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

        },
    });
});

//
// Interface HTMLMediaElement
//

// Constants defined by HTMLMediaElement
const NETWORK_EMPTY = 0;
const NETWORK_IDLE = 1;
const NETWORK_LOADING = 2;
const NETWORK_NO_SOURCE = 3;
const HAVE_NOTHING = 0;
const HAVE_METADATA = 1;
const HAVE_CURRENT_DATA = 2;
const HAVE_FUTURE_DATA = 3;
const HAVE_ENOUGH_DATA = 4;

defineLazyProperty(global, "HTMLMediaElement", function() {
    return idl.HTMLMediaElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLMediaElement", function() {
    return new IDLInterface({
        name: "HTMLMediaElement",
        superclass: idl.HTMLElement,
        constants: {
            NETWORK_EMPTY: NETWORK_EMPTY,
            NETWORK_IDLE: NETWORK_IDLE,
            NETWORK_LOADING: NETWORK_LOADING,
            NETWORK_NO_SOURCE: NETWORK_NO_SOURCE,
            HAVE_NOTHING: HAVE_NOTHING,
            HAVE_METADATA: HAVE_METADATA,
            HAVE_CURRENT_DATA: HAVE_CURRENT_DATA,
            HAVE_FUTURE_DATA: HAVE_FUTURE_DATA,
            HAVE_ENOUGH_DATA: HAVE_ENOUGH_DATA,
        },
        members: {
            get error() {
                return wrap(unwrap(this).error);
            },

            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get currentSrc() {
                return unwrap(this).currentSrc;
            },

            get crossOrigin() {
                return unwrap(this).crossOrigin;
            },
            set crossOrigin(newval) {
                unwrap(this).crossOrigin = String(newval);
            },

            get networkState() {
                return unwrap(this).networkState;
            },

            get preload() {
                return unwrap(this).preload;
            },
            set preload(newval) {
                unwrap(this).preload = String(newval);
            },

            get buffered() {
                return wrap(unwrap(this).buffered);
            },

            load: function load() {
                unwrap(this).load();
            },

            canPlayType: function canPlayType(type) {
                return unwrap(this).canPlayType(String(type));
            },

            get readyState() {
                return unwrap(this).readyState;
            },

            get seeking() {
                return unwrap(this).seeking;
            },

            get currentTime() {
                return unwrap(this).currentTime;
            },
            set currentTime(newval) {
                unwrap(this).currentTime = toDouble(newval);
            },

            get initialTime() {
                return unwrap(this).initialTime;
            },

            get duration() {
                return unwrap(this).duration;
            },

            get startOffsetTime() {
                return wrap(unwrap(this).startOffsetTime);
            },

            get paused() {
                return unwrap(this).paused;
            },

            get defaultPlaybackRate() {
                return unwrap(this).defaultPlaybackRate;
            },
            set defaultPlaybackRate(newval) {
                unwrap(this).defaultPlaybackRate = toDouble(newval);
            },

            get playbackRate() {
                return unwrap(this).playbackRate;
            },
            set playbackRate(newval) {
                unwrap(this).playbackRate = toDouble(newval);
            },

            get played() {
                return wrap(unwrap(this).played);
            },

            get seekable() {
                return wrap(unwrap(this).seekable);
            },

            get ended() {
                return unwrap(this).ended;
            },

            get autoplay() {
                return unwrap(this).autoplay;
            },
            set autoplay(newval) {
                unwrap(this).autoplay = Boolean(newval);
            },

            get loop() {
                return unwrap(this).loop;
            },
            set loop(newval) {
                unwrap(this).loop = Boolean(newval);
            },

            play: function play() {
                unwrap(this).play();
            },

            pause: function pause() {
                unwrap(this).pause();
            },

            get mediaGroup() {
                return unwrap(this).mediaGroup;
            },
            set mediaGroup(newval) {
                unwrap(this).mediaGroup = String(newval);
            },

            get controller() {
                return wrap(unwrap(this).controller);
            },
            set controller(newval) {
                unwrap(this).controller = unwrapOrNull(newval);
            },

            get controls() {
                return unwrap(this).controls;
            },
            set controls(newval) {
                unwrap(this).controls = Boolean(newval);
            },

            get volume() {
                return unwrap(this).volume;
            },
            set volume(newval) {
                unwrap(this).volume = toDouble(newval);
            },

            get muted() {
                return unwrap(this).muted;
            },
            set muted(newval) {
                unwrap(this).muted = Boolean(newval);
            },

            get defaultMuted() {
                return unwrap(this).defaultMuted;
            },
            set defaultMuted(newval) {
                unwrap(this).defaultMuted = Boolean(newval);
            },

            get audioTracks() {
                return wrap(unwrap(this).audioTracks);
            },

            get videoTracks() {
                return wrap(unwrap(this).videoTracks);
            },

            get textTracks() {
                return wrap(unwrap(this).textTracks);
            },

            addTextTrack: function addTextTrack(
                                    kind,
                                    label,
                                    language)
            {
                return wrap(unwrap(this).addTextTrack(
                    String(kind),
                    OptionalString(label),
                    OptionalString(language)));
            },

        },
    });
});

//
// Interface HTMLVideoElement
//

defineLazyProperty(global, "HTMLVideoElement", function() {
    return idl.HTMLVideoElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLVideoElement", function() {
    return new IDLInterface({
        name: "HTMLVideoElement",
        superclass: idl.HTMLMediaElement,
        members: {
            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = toULong(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = toULong(newval);
            },

            get videoWidth() {
                return unwrap(this).videoWidth;
            },

            get videoHeight() {
                return unwrap(this).videoHeight;
            },

            get poster() {
                return unwrap(this).poster;
            },
            set poster(newval) {
                unwrap(this).poster = String(newval);
            },

        },
    });
});

//
// Interface HTMLAudioElement
//

defineLazyProperty(global, "HTMLAudioElement", function() {
    return idl.HTMLAudioElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLAudioElement", function() {
    return new IDLInterface({
        name: "HTMLAudioElement",
        superclass: idl.HTMLMediaElement,
        members: {
        },
    });
});

//
// Interface HTMLSourceElement
//

defineLazyProperty(global, "HTMLSourceElement", function() {
    return idl.HTMLSourceElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLSourceElement", function() {
    return new IDLInterface({
        name: "HTMLSourceElement",
        superclass: idl.HTMLElement,
        members: {
            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get media() {
                return unwrap(this).media;
            },
            set media(newval) {
                unwrap(this).media = String(newval);
            },

        },
    });
});

//
// Interface HTMLTrackElement
//

defineLazyProperty(global, "HTMLTrackElement", function() {
    return idl.HTMLTrackElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTrackElement", function() {
    return new IDLInterface({
        name: "HTMLTrackElement",
        superclass: idl.HTMLElement,
        members: {
            get kind() {
                return unwrap(this).kind;
            },
            set kind(newval) {
                unwrap(this).kind = String(newval);
            },

            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get srclang() {
                return unwrap(this).srclang;
            },
            set srclang(newval) {
                unwrap(this).srclang = String(newval);
            },

            get label() {
                return unwrap(this).label;
            },
            set label(newval) {
                unwrap(this).label = String(newval);
            },

            get default() {
                return unwrap(this).default;
            },
            set default(newval) {
                unwrap(this).default = Boolean(newval);
            },

            get track() {
                return wrap(unwrap(this).track);
            },

        },
    });
});

//
// Interface HTMLMapElement
//

defineLazyProperty(global, "HTMLMapElement", function() {
    return idl.HTMLMapElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLMapElement", function() {
    return new IDLInterface({
        name: "HTMLMapElement",
        superclass: idl.HTMLElement,
        members: {
            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get areas() {
                return wrap(unwrap(this).areas);
            },

            get images() {
                return wrap(unwrap(this).images);
            },

        },
    });
});

//
// Interface HTMLAreaElement
//

defineLazyProperty(global, "HTMLAreaElement", function() {
    return idl.HTMLAreaElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLAreaElement", function() {
    return new IDLInterface({
        name: "HTMLAreaElement",
        superclass: idl.HTMLElement,
        members: {
            get alt() {
                return unwrap(this).alt;
            },
            set alt(newval) {
                unwrap(this).alt = String(newval);
            },

            get coords() {
                return unwrap(this).coords;
            },
            set coords(newval) {
                unwrap(this).coords = String(newval);
            },

            get shape() {
                return unwrap(this).shape;
            },
            set shape(newval) {
                unwrap(this).shape = String(newval);
            },

            get href() {
                return unwrap(this).href;
            },
            set href(newval) {
                unwrap(this).href = String(newval);
            },

            get target() {
                return unwrap(this).target;
            },
            set target(newval) {
                unwrap(this).target = String(newval);
            },

            get download() {
                return unwrap(this).download;
            },
            set download(newval) {
                unwrap(this).download = String(newval);
            },

            get ping() {
                return unwrap(this).ping;
            },
            set ping(newval) {
                unwrap(this).ping = String(newval);
            },

            get rel() {
                return unwrap(this).rel;
            },
            set rel(newval) {
                unwrap(this).rel = String(newval);
            },

            get relList() {
                return wrap(unwrap(this).relList);
            },

            get media() {
                return unwrap(this).media;
            },
            set media(newval) {
                unwrap(this).media = String(newval);
            },

            get hreflang() {
                return unwrap(this).hreflang;
            },
            set hreflang(newval) {
                unwrap(this).hreflang = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get protocol() {
                return unwrap(this).protocol;
            },
            set protocol(newval) {
                unwrap(this).protocol = String(newval);
            },

            get host() {
                return unwrap(this).host;
            },
            set host(newval) {
                unwrap(this).host = String(newval);
            },

            get hostname() {
                return unwrap(this).hostname;
            },
            set hostname(newval) {
                unwrap(this).hostname = String(newval);
            },

            get port() {
                return unwrap(this).port;
            },
            set port(newval) {
                unwrap(this).port = String(newval);
            },

            get pathname() {
                return unwrap(this).pathname;
            },
            set pathname(newval) {
                unwrap(this).pathname = String(newval);
            },

            get search() {
                return unwrap(this).search;
            },
            set search(newval) {
                unwrap(this).search = String(newval);
            },

            get hash() {
                return unwrap(this).hash;
            },
            set hash(newval) {
                unwrap(this).hash = String(newval);
            },

        },
    });
});

//
// Interface HTMLTableElement
//

defineLazyProperty(global, "HTMLTableElement", function() {
    return idl.HTMLTableElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableElement", function() {
    return new IDLInterface({
        name: "HTMLTableElement",
        superclass: idl.HTMLElement,
        members: {
            get caption() {
                return wrap(unwrap(this).caption);
            },
            set caption(newval) {
                unwrap(this).caption = unwrapOrNull(newval);
            },

            createCaption: function createCaption() {
                return wrap(unwrap(this).createCaption());
            },

            deleteCaption: function deleteCaption() {
                unwrap(this).deleteCaption();
            },

            get tHead() {
                return wrap(unwrap(this).tHead);
            },
            set tHead(newval) {
                unwrap(this).tHead = unwrapOrNull(newval);
            },

            createTHead: function createTHead() {
                return wrap(unwrap(this).createTHead());
            },

            deleteTHead: function deleteTHead() {
                unwrap(this).deleteTHead();
            },

            get tFoot() {
                return wrap(unwrap(this).tFoot);
            },
            set tFoot(newval) {
                unwrap(this).tFoot = unwrapOrNull(newval);
            },

            createTFoot: function createTFoot() {
                return wrap(unwrap(this).createTFoot());
            },

            deleteTFoot: function deleteTFoot() {
                unwrap(this).deleteTFoot();
            },

            get tBodies() {
                return wrap(unwrap(this).tBodies);
            },

            createTBody: function createTBody() {
                return wrap(unwrap(this).createTBody());
            },

            get rows() {
                return wrap(unwrap(this).rows);
            },

            insertRow: function insertRow(index) {
                return wrap(unwrap(this).insertRow(OptionaltoLong(index)));
            },

            deleteRow: function deleteRow(index) {
                unwrap(this).deleteRow(toLong(index));
            },

            get border() {
                return unwrap(this).border;
            },
            set border(newval) {
                unwrap(this).border = String(newval);
            },

        },
    });
});

//
// Interface HTMLTableCaptionElement
//

defineLazyProperty(global, "HTMLTableCaptionElement", function() {
    return idl.HTMLTableCaptionElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableCaptionElement", function() {
    return new IDLInterface({
        name: "HTMLTableCaptionElement",
        superclass: idl.HTMLElement,
        members: {
        },
    });
});

//
// Interface HTMLTableColElement
//

defineLazyProperty(global, "HTMLTableColElement", function() {
    return idl.HTMLTableColElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableColElement", function() {
    return new IDLInterface({
        name: "HTMLTableColElement",
        superclass: idl.HTMLElement,
        members: {
            get span() {
                return unwrap(this).span;
            },
            set span(newval) {
                unwrap(this).span = toULong(newval);
            },

        },
    });
});

//
// Interface HTMLTableSectionElement
//

defineLazyProperty(global, "HTMLTableSectionElement", function() {
    return idl.HTMLTableSectionElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableSectionElement", function() {
    return new IDLInterface({
        name: "HTMLTableSectionElement",
        superclass: idl.HTMLElement,
        members: {
            get rows() {
                return wrap(unwrap(this).rows);
            },

            insertRow: function insertRow(index) {
                return wrap(unwrap(this).insertRow(OptionaltoLong(index)));
            },

            deleteRow: function deleteRow(index) {
                unwrap(this).deleteRow(toLong(index));
            },

        },
    });
});

//
// Interface HTMLTableRowElement
//

defineLazyProperty(global, "HTMLTableRowElement", function() {
    return idl.HTMLTableRowElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableRowElement", function() {
    return new IDLInterface({
        name: "HTMLTableRowElement",
        superclass: idl.HTMLElement,
        members: {
            get rowIndex() {
                return unwrap(this).rowIndex;
            },

            get sectionRowIndex() {
                return unwrap(this).sectionRowIndex;
            },

            get cells() {
                return wrap(unwrap(this).cells);
            },

            insertCell: function insertCell(index) {
                return wrap(unwrap(this).insertCell(OptionaltoLong(index)));
            },

            deleteCell: function deleteCell(index) {
                unwrap(this).deleteCell(toLong(index));
            },

        },
    });
});

//
// Interface HTMLTableDataCellElement
//

defineLazyProperty(global, "HTMLTableDataCellElement", function() {
    return idl.HTMLTableDataCellElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableDataCellElement", function() {
    return new IDLInterface({
        name: "HTMLTableDataCellElement",
        superclass: idl.HTMLTableCellElement,
        members: {
        },
    });
});

//
// Interface HTMLTableHeaderCellElement
//

defineLazyProperty(global, "HTMLTableHeaderCellElement", function() {
    return idl.HTMLTableHeaderCellElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableHeaderCellElement", function() {
    return new IDLInterface({
        name: "HTMLTableHeaderCellElement",
        superclass: idl.HTMLTableCellElement,
        members: {
            get scope() {
                return unwrap(this).scope;
            },
            set scope(newval) {
                unwrap(this).scope = String(newval);
            },

        },
    });
});

//
// Interface HTMLTableCellElement
//

defineLazyProperty(global, "HTMLTableCellElement", function() {
    return idl.HTMLTableCellElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTableCellElement", function() {
    return new IDLInterface({
        name: "HTMLTableCellElement",
        superclass: idl.HTMLElement,
        members: {
            get colSpan() {
                return unwrap(this).colSpan;
            },
            set colSpan(newval) {
                unwrap(this).colSpan = toULong(newval);
            },

            get rowSpan() {
                return unwrap(this).rowSpan;
            },
            set rowSpan(newval) {
                unwrap(this).rowSpan = toULong(newval);
            },

            get headers() {
                return wrap(unwrap(this).headers);
            },

            get cellIndex() {
                return unwrap(this).cellIndex;
            },

        },
    });
});

//
// Interface HTMLFormElement
//

defineLazyProperty(global, "HTMLFormElement", function() {
    return idl.HTMLFormElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLFormElement", function() {
    return new IDLInterface({
        name: "HTMLFormElement",
        superclass: idl.HTMLElement,
        members: {
            get acceptCharset() {
                return unwrap(this).acceptCharset;
            },
            set acceptCharset(newval) {
                unwrap(this).acceptCharset = String(newval);
            },

            get action() {
                return unwrap(this).action;
            },
            set action(newval) {
                unwrap(this).action = String(newval);
            },

            get autocomplete() {
                return unwrap(this).autocomplete;
            },
            set autocomplete(newval) {
                unwrap(this).autocomplete = String(newval);
            },

            get enctype() {
                return unwrap(this).enctype;
            },
            set enctype(newval) {
                unwrap(this).enctype = String(newval);
            },

            get encoding() {
                return unwrap(this).encoding;
            },
            set encoding(newval) {
                unwrap(this).encoding = String(newval);
            },

            get method() {
                return unwrap(this).method;
            },
            set method(newval) {
                unwrap(this).method = String(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get noValidate() {
                return unwrap(this).noValidate;
            },
            set noValidate(newval) {
                unwrap(this).noValidate = Boolean(newval);
            },

            get target() {
                return unwrap(this).target;
            },
            set target(newval) {
                unwrap(this).target = String(newval);
            },

            get elements() {
                return wrap(unwrap(this).elements);
            },

            get length() {
                return unwrap(this).length;
            },

            submit: function submit() {
                unwrap(this).submit();
            },

            reset: function reset() {
                unwrap(this).reset();
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

        },
    });
});

//
// Interface HTMLFieldSetElement
//

defineLazyProperty(global, "HTMLFieldSetElement", function() {
    return idl.HTMLFieldSetElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLFieldSetElement", function() {
    return new IDLInterface({
        name: "HTMLFieldSetElement",
        superclass: idl.HTMLElement,
        members: {
            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },

            get elements() {
                return wrap(unwrap(this).elements);
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

        },
    });
});

//
// Interface HTMLLegendElement
//

defineLazyProperty(global, "HTMLLegendElement", function() {
    return idl.HTMLLegendElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLLegendElement", function() {
    return new IDLInterface({
        name: "HTMLLegendElement",
        superclass: idl.HTMLElement,
        members: {
            get form() {
                return wrap(unwrap(this).form);
            },

        },
    });
});

//
// Interface HTMLLabelElement
//

defineLazyProperty(global, "HTMLLabelElement", function() {
    return idl.HTMLLabelElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLLabelElement", function() {
    return new IDLInterface({
        name: "HTMLLabelElement",
        superclass: idl.HTMLElement,
        members: {
            get form() {
                return wrap(unwrap(this).form);
            },

            get htmlFor() {
                return unwrap(this).htmlFor;
            },
            set htmlFor(newval) {
                unwrap(this).htmlFor = String(newval);
            },

            get control() {
                return wrap(unwrap(this).control);
            },

        },
    });
});

//
// Interface HTMLInputElement
//

defineLazyProperty(global, "HTMLInputElement", function() {
    return idl.HTMLInputElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLInputElement", function() {
    return new IDLInterface({
        name: "HTMLInputElement",
        superclass: idl.HTMLElement,
        members: {
            get accept() {
                return unwrap(this).accept;
            },
            set accept(newval) {
                unwrap(this).accept = String(newval);
            },

            get alt() {
                return unwrap(this).alt;
            },
            set alt(newval) {
                unwrap(this).alt = String(newval);
            },

            get autocomplete() {
                return unwrap(this).autocomplete;
            },
            set autocomplete(newval) {
                unwrap(this).autocomplete = String(newval);
            },

            get autofocus() {
                return unwrap(this).autofocus;
            },
            set autofocus(newval) {
                unwrap(this).autofocus = Boolean(newval);
            },

            get defaultChecked() {
                return unwrap(this).defaultChecked;
            },
            set defaultChecked(newval) {
                unwrap(this).defaultChecked = Boolean(newval);
            },

            get checked() {
                return unwrap(this).checked;
            },
            set checked(newval) {
                unwrap(this).checked = Boolean(newval);
            },

            get dirName() {
                return unwrap(this).dirName;
            },
            set dirName(newval) {
                unwrap(this).dirName = String(newval);
            },

            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get files() {
                return wrap(unwrap(this).files);
            },

            get formAction() {
                return unwrap(this).formAction;
            },
            set formAction(newval) {
                unwrap(this).formAction = String(newval);
            },

            get formEnctype() {
                return unwrap(this).formEnctype;
            },
            set formEnctype(newval) {
                unwrap(this).formEnctype = String(newval);
            },

            get formMethod() {
                return unwrap(this).formMethod;
            },
            set formMethod(newval) {
                unwrap(this).formMethod = String(newval);
            },

            get formNoValidate() {
                return unwrap(this).formNoValidate;
            },
            set formNoValidate(newval) {
                unwrap(this).formNoValidate = Boolean(newval);
            },

            get formTarget() {
                return unwrap(this).formTarget;
            },
            set formTarget(newval) {
                unwrap(this).formTarget = String(newval);
            },

            get height() {
                return unwrap(this).height;
            },
            set height(newval) {
                unwrap(this).height = String(newval);
            },

            get indeterminate() {
                return unwrap(this).indeterminate;
            },
            set indeterminate(newval) {
                unwrap(this).indeterminate = Boolean(newval);
            },

            get list() {
                return wrap(unwrap(this).list);
            },

            get max() {
                return unwrap(this).max;
            },
            set max(newval) {
                unwrap(this).max = String(newval);
            },

            get maxLength() {
                return unwrap(this).maxLength;
            },
            set maxLength(newval) {
                unwrap(this).maxLength = toLong(newval);
            },

            get min() {
                return unwrap(this).min;
            },
            set min(newval) {
                unwrap(this).min = String(newval);
            },

            get multiple() {
                return unwrap(this).multiple;
            },
            set multiple(newval) {
                unwrap(this).multiple = Boolean(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get pattern() {
                return unwrap(this).pattern;
            },
            set pattern(newval) {
                unwrap(this).pattern = String(newval);
            },

            get placeholder() {
                return unwrap(this).placeholder;
            },
            set placeholder(newval) {
                unwrap(this).placeholder = String(newval);
            },

            get readOnly() {
                return unwrap(this).readOnly;
            },
            set readOnly(newval) {
                unwrap(this).readOnly = Boolean(newval);
            },

            get required() {
                return unwrap(this).required;
            },
            set required(newval) {
                unwrap(this).required = Boolean(newval);
            },

            get size() {
                return unwrap(this).size;
            },
            set size(newval) {
                unwrap(this).size = toULong(newval);
            },

            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get step() {
                return unwrap(this).step;
            },
            set step(newval) {
                unwrap(this).step = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get defaultValue() {
                return unwrap(this).defaultValue;
            },
            set defaultValue(newval) {
                unwrap(this).defaultValue = String(newval);
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

            get valueAsDate() {
                return wrap(unwrap(this).valueAsDate);
            },
            set valueAsDate(newval) {
                unwrap(this).valueAsDate = unwrap(newval);
            },

            get valueAsNumber() {
                return unwrap(this).valueAsNumber;
            },
            set valueAsNumber(newval) {
                unwrap(this).valueAsNumber = toDouble(newval);
            },

            get selectedOption() {
                return wrap(unwrap(this).selectedOption);
            },

            get width() {
                return unwrap(this).width;
            },
            set width(newval) {
                unwrap(this).width = String(newval);
            },

            stepUp: function stepUp(n) {
                unwrap(this).stepUp(OptionaltoLong(n));
            },

            stepDown: function stepDown(n) {
                unwrap(this).stepDown(OptionaltoLong(n));
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

            select: function select() {
                unwrap(this).select();
            },

            get selectionStart() {
                return unwrap(this).selectionStart;
            },
            set selectionStart(newval) {
                unwrap(this).selectionStart = toULong(newval);
            },

            get selectionEnd() {
                return unwrap(this).selectionEnd;
            },
            set selectionEnd(newval) {
                unwrap(this).selectionEnd = toULong(newval);
            },

            get selectionDirection() {
                return unwrap(this).selectionDirection;
            },
            set selectionDirection(newval) {
                unwrap(this).selectionDirection = String(newval);
            },

            setSelectionRange: function setSelectionRange(
                                    start,
                                    end,
                                    direction)
            {
                unwrap(this).setSelectionRange(
                    toULong(start),
                    toULong(end),
                    OptionalString(direction));
            },

        },
    });
});

//
// Interface HTMLButtonElement
//

defineLazyProperty(global, "HTMLButtonElement", function() {
    return idl.HTMLButtonElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLButtonElement", function() {
    return new IDLInterface({
        name: "HTMLButtonElement",
        superclass: idl.HTMLElement,
        members: {
            get autofocus() {
                return unwrap(this).autofocus;
            },
            set autofocus(newval) {
                unwrap(this).autofocus = Boolean(newval);
            },

            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get formAction() {
                return unwrap(this).formAction;
            },
            set formAction(newval) {
                unwrap(this).formAction = String(newval);
            },

            get formEnctype() {
                return unwrap(this).formEnctype;
            },
            set formEnctype(newval) {
                unwrap(this).formEnctype = String(newval);
            },

            get formMethod() {
                return unwrap(this).formMethod;
            },
            set formMethod(newval) {
                unwrap(this).formMethod = String(newval);
            },

            get formNoValidate() {
                return unwrap(this).formNoValidate;
            },
            set formNoValidate(newval) {
                unwrap(this).formNoValidate = Boolean(newval);
            },

            get formTarget() {
                return unwrap(this).formTarget;
            },
            set formTarget(newval) {
                unwrap(this).formTarget = String(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

        },
    });
});

//
// Interface HTMLSelectElement
//

defineLazyProperty(global, "HTMLSelectElement", function() {
    return idl.HTMLSelectElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLSelectElement", function() {
    return new IDLInterface({
        name: "HTMLSelectElement",
        superclass: idl.HTMLElement,
        members: {
            get autofocus() {
                return unwrap(this).autofocus;
            },
            set autofocus(newval) {
                unwrap(this).autofocus = Boolean(newval);
            },

            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get multiple() {
                return unwrap(this).multiple;
            },
            set multiple(newval) {
                unwrap(this).multiple = Boolean(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get required() {
                return unwrap(this).required;
            },
            set required(newval) {
                unwrap(this).required = Boolean(newval);
            },

            get size() {
                return unwrap(this).size;
            },
            set size(newval) {
                unwrap(this).size = toULong(newval);
            },

            get type() {
                return unwrap(this).type;
            },

            get options() {
                return wrap(unwrap(this).options);
            },

            get length() {
                return unwrap(this).length;
            },
            set length(newval) {
                unwrap(this).length = toULong(newval);
            },

            namedItem: function namedItem(name) {
                return unwrap(this).namedItem(String(name));
            },

            remove: function remove(index) {
                unwrap(this).remove(toLong(index));
            },

            get selectedOptions() {
                return wrap(unwrap(this).selectedOptions);
            },

            get selectedIndex() {
                return unwrap(this).selectedIndex;
            },
            set selectedIndex(newval) {
                unwrap(this).selectedIndex = toLong(newval);
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

        },
    });
});

//
// Interface HTMLDataListElement
//

defineLazyProperty(global, "HTMLDataListElement", function() {
    return idl.HTMLDataListElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLDataListElement", function() {
    return new IDLInterface({
        name: "HTMLDataListElement",
        superclass: idl.HTMLElement,
        members: {
            get options() {
                return wrap(unwrap(this).options);
            },

        },
    });
});

//
// Interface HTMLOptGroupElement
//

defineLazyProperty(global, "HTMLOptGroupElement", function() {
    return idl.HTMLOptGroupElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLOptGroupElement", function() {
    return new IDLInterface({
        name: "HTMLOptGroupElement",
        superclass: idl.HTMLElement,
        members: {
            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get label() {
                return unwrap(this).label;
            },
            set label(newval) {
                unwrap(this).label = String(newval);
            },

        },
    });
});

//
// Interface HTMLOptionElement
//

defineLazyProperty(global, "HTMLOptionElement", function() {
    return idl.HTMLOptionElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLOptionElement", function() {
    return new IDLInterface({
        name: "HTMLOptionElement",
        superclass: idl.HTMLElement,
        members: {
            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get label() {
                return unwrap(this).label;
            },
            set label(newval) {
                unwrap(this).label = String(newval);
            },

            get defaultSelected() {
                return unwrap(this).defaultSelected;
            },
            set defaultSelected(newval) {
                unwrap(this).defaultSelected = Boolean(newval);
            },

            get selected() {
                return unwrap(this).selected;
            },
            set selected(newval) {
                unwrap(this).selected = Boolean(newval);
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

            get text() {
                return unwrap(this).text;
            },
            set text(newval) {
                unwrap(this).text = String(newval);
            },

            get index() {
                return unwrap(this).index;
            },

        },
    });
});

//
// Interface HTMLTextAreaElement
//

defineLazyProperty(global, "HTMLTextAreaElement", function() {
    return idl.HTMLTextAreaElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLTextAreaElement", function() {
    return new IDLInterface({
        name: "HTMLTextAreaElement",
        superclass: idl.HTMLElement,
        members: {
            get autofocus() {
                return unwrap(this).autofocus;
            },
            set autofocus(newval) {
                unwrap(this).autofocus = Boolean(newval);
            },

            get cols() {
                return unwrap(this).cols;
            },
            set cols(newval) {
                unwrap(this).cols = toULong(newval);
            },

            get dirName() {
                return unwrap(this).dirName;
            },
            set dirName(newval) {
                unwrap(this).dirName = String(newval);
            },

            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get maxLength() {
                return unwrap(this).maxLength;
            },
            set maxLength(newval) {
                unwrap(this).maxLength = toLong(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get placeholder() {
                return unwrap(this).placeholder;
            },
            set placeholder(newval) {
                unwrap(this).placeholder = String(newval);
            },

            get readOnly() {
                return unwrap(this).readOnly;
            },
            set readOnly(newval) {
                unwrap(this).readOnly = Boolean(newval);
            },

            get required() {
                return unwrap(this).required;
            },
            set required(newval) {
                unwrap(this).required = Boolean(newval);
            },

            get rows() {
                return unwrap(this).rows;
            },
            set rows(newval) {
                unwrap(this).rows = toULong(newval);
            },

            get wrap() {
                return unwrap(this).wrap;
            },
            set wrap(newval) {
                unwrap(this).wrap = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },

            get defaultValue() {
                return unwrap(this).defaultValue;
            },
            set defaultValue(newval) {
                unwrap(this).defaultValue = String(newval);
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

            get textLength() {
                return unwrap(this).textLength;
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

            select: function select() {
                unwrap(this).select();
            },

            get selectionStart() {
                return unwrap(this).selectionStart;
            },
            set selectionStart(newval) {
                unwrap(this).selectionStart = toULong(newval);
            },

            get selectionEnd() {
                return unwrap(this).selectionEnd;
            },
            set selectionEnd(newval) {
                unwrap(this).selectionEnd = toULong(newval);
            },

            get selectionDirection() {
                return unwrap(this).selectionDirection;
            },
            set selectionDirection(newval) {
                unwrap(this).selectionDirection = String(newval);
            },

            setSelectionRange: function setSelectionRange(
                                    start,
                                    end,
                                    direction)
            {
                unwrap(this).setSelectionRange(
                    toULong(start),
                    toULong(end),
                    OptionalString(direction));
            },

        },
    });
});

//
// Interface HTMLKeygenElement
//

defineLazyProperty(global, "HTMLKeygenElement", function() {
    return idl.HTMLKeygenElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLKeygenElement", function() {
    return new IDLInterface({
        name: "HTMLKeygenElement",
        superclass: idl.HTMLElement,
        members: {
            get autofocus() {
                return unwrap(this).autofocus;
            },
            set autofocus(newval) {
                unwrap(this).autofocus = Boolean(newval);
            },

            get challenge() {
                return unwrap(this).challenge;
            },
            set challenge(newval) {
                unwrap(this).challenge = String(newval);
            },

            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get keytype() {
                return unwrap(this).keytype;
            },
            set keytype(newval) {
                unwrap(this).keytype = String(newval);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

        },
    });
});

//
// Interface HTMLOutputElement
//

defineLazyProperty(global, "HTMLOutputElement", function() {
    return idl.HTMLOutputElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLOutputElement", function() {
    return new IDLInterface({
        name: "HTMLOutputElement",
        superclass: idl.HTMLElement,
        members: {
            get htmlFor() {
                return wrap(unwrap(this).htmlFor);
            },

            get form() {
                return wrap(unwrap(this).form);
            },

            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get type() {
                return unwrap(this).type;
            },

            get defaultValue() {
                return unwrap(this).defaultValue;
            },
            set defaultValue(newval) {
                unwrap(this).defaultValue = String(newval);
            },

            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = String(newval);
            },

            get willValidate() {
                return unwrap(this).willValidate;
            },

            get validity() {
                return wrap(unwrap(this).validity);
            },

            get validationMessage() {
                return unwrap(this).validationMessage;
            },

            checkValidity: function checkValidity() {
                return unwrap(this).checkValidity();
            },

            setCustomValidity: function setCustomValidity(error) {
                unwrap(this).setCustomValidity(String(error));
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

        },
    });
});

//
// Interface HTMLProgressElement
//

defineLazyProperty(global, "HTMLProgressElement", function() {
    return idl.HTMLProgressElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLProgressElement", function() {
    return new IDLInterface({
        name: "HTMLProgressElement",
        superclass: idl.HTMLElement,
        members: {
            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = toDouble(newval);
            },

            get max() {
                return unwrap(this).max;
            },
            set max(newval) {
                unwrap(this).max = toDouble(newval);
            },

            get position() {
                return unwrap(this).position;
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

        },
    });
});

//
// Interface HTMLMeterElement
//

defineLazyProperty(global, "HTMLMeterElement", function() {
    return idl.HTMLMeterElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLMeterElement", function() {
    return new IDLInterface({
        name: "HTMLMeterElement",
        superclass: idl.HTMLElement,
        members: {
            get value() {
                return unwrap(this).value;
            },
            set value(newval) {
                unwrap(this).value = toDouble(newval);
            },

            get min() {
                return unwrap(this).min;
            },
            set min(newval) {
                unwrap(this).min = toDouble(newval);
            },

            get max() {
                return unwrap(this).max;
            },
            set max(newval) {
                unwrap(this).max = toDouble(newval);
            },

            get low() {
                return unwrap(this).low;
            },
            set low(newval) {
                unwrap(this).low = toDouble(newval);
            },

            get high() {
                return unwrap(this).high;
            },
            set high(newval) {
                unwrap(this).high = toDouble(newval);
            },

            get optimum() {
                return unwrap(this).optimum;
            },
            set optimum(newval) {
                unwrap(this).optimum = toDouble(newval);
            },

            get labels() {
                return wrap(unwrap(this).labels);
            },

        },
    });
});

//
// Interface ValidityState
//

defineLazyProperty(global, "ValidityState", function() {
    return idl.ValidityState.publicInterface;
}, true);

defineLazyProperty(idl, "ValidityState", function() {
    return new IDLInterface({
        name: "ValidityState",
        members: {
            get valueMissing() {
                return unwrap(this).valueMissing;
            },

            get typeMismatch() {
                return unwrap(this).typeMismatch;
            },

            get patternMismatch() {
                return unwrap(this).patternMismatch;
            },

            get tooLong() {
                return unwrap(this).tooLong;
            },

            get rangeUnderflow() {
                return unwrap(this).rangeUnderflow;
            },

            get rangeOverflow() {
                return unwrap(this).rangeOverflow;
            },

            get stepMismatch() {
                return unwrap(this).stepMismatch;
            },

            get customError() {
                return unwrap(this).customError;
            },

            get valid() {
                return unwrap(this).valid;
            },

        },
    });
});

//
// Interface HTMLDetailsElement
//

defineLazyProperty(global, "HTMLDetailsElement", function() {
    return idl.HTMLDetailsElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLDetailsElement", function() {
    return new IDLInterface({
        name: "HTMLDetailsElement",
        superclass: idl.HTMLElement,
        members: {
            get open() {
                return unwrap(this).open;
            },
            set open(newval) {
                unwrap(this).open = Boolean(newval);
            },

        },
    });
});

//
// Interface HTMLCommandElement
//

defineLazyProperty(global, "HTMLCommandElement", function() {
    return idl.HTMLCommandElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLCommandElement", function() {
    return new IDLInterface({
        name: "HTMLCommandElement",
        superclass: idl.HTMLElement,
        members: {
            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get label() {
                return unwrap(this).label;
            },
            set label(newval) {
                unwrap(this).label = String(newval);
            },

            get icon() {
                return unwrap(this).icon;
            },
            set icon(newval) {
                unwrap(this).icon = String(newval);
            },

            get disabled() {
                return unwrap(this).disabled;
            },
            set disabled(newval) {
                unwrap(this).disabled = Boolean(newval);
            },

            get checked() {
                return unwrap(this).checked;
            },
            set checked(newval) {
                unwrap(this).checked = Boolean(newval);
            },

            get radiogroup() {
                return unwrap(this).radiogroup;
            },
            set radiogroup(newval) {
                unwrap(this).radiogroup = String(newval);
            },

        },
    });
});

//
// Interface HTMLMenuElement
//

defineLazyProperty(global, "HTMLMenuElement", function() {
    return idl.HTMLMenuElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLMenuElement", function() {
    return new IDLInterface({
        name: "HTMLMenuElement",
        superclass: idl.HTMLElement,
        members: {
            get type() {
                return unwrap(this).type;
            },
            set type(newval) {
                unwrap(this).type = String(newval);
            },

            get label() {
                return unwrap(this).label;
            },
            set label(newval) {
                unwrap(this).label = String(newval);
            },

        },
    });
});

//
// Interface HTMLFrameSetElement
//

defineLazyProperty(global, "HTMLFrameSetElement", function() {
    return idl.HTMLFrameSetElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLFrameSetElement", function() {
    return new IDLInterface({
        name: "HTMLFrameSetElement",
        superclass: idl.HTMLElement,
        members: {
            get cols() {
                return unwrap(this).cols;
            },
            set cols(newval) {
                unwrap(this).cols = String(newval);
            },

            get rows() {
                return unwrap(this).rows;
            },
            set rows(newval) {
                unwrap(this).rows = String(newval);
            },

            get onafterprint() {
                return unwrap(this).onafterprint;
            },
            set onafterprint(newval) {
                unwrap(this).onafterprint = toCallbackOrNull(newval);
            },

            get onbeforeprint() {
                return unwrap(this).onbeforeprint;
            },
            set onbeforeprint(newval) {
                unwrap(this).onbeforeprint = toCallbackOrNull(newval);
            },

            get onbeforeunload() {
                return unwrap(this).onbeforeunload;
            },
            set onbeforeunload(newval) {
                unwrap(this).onbeforeunload = toCallbackOrNull(newval);
            },

            get onblur() {
                return unwrap(this).onblur;
            },
            set onblur(newval) {
                unwrap(this).onblur = toCallbackOrNull(newval);
            },

            get onerror() {
                return unwrap(this).onerror;
            },
            set onerror(newval) {
                unwrap(this).onerror = toCallbackOrNull(newval);
            },

            get onfocus() {
                return unwrap(this).onfocus;
            },
            set onfocus(newval) {
                unwrap(this).onfocus = toCallbackOrNull(newval);
            },

            get onhashchange() {
                return unwrap(this).onhashchange;
            },
            set onhashchange(newval) {
                unwrap(this).onhashchange = toCallbackOrNull(newval);
            },

            get onload() {
                return unwrap(this).onload;
            },
            set onload(newval) {
                unwrap(this).onload = toCallbackOrNull(newval);
            },

            get onmessage() {
                return unwrap(this).onmessage;
            },
            set onmessage(newval) {
                unwrap(this).onmessage = toCallbackOrNull(newval);
            },

            get onoffline() {
                return unwrap(this).onoffline;
            },
            set onoffline(newval) {
                unwrap(this).onoffline = toCallbackOrNull(newval);
            },

            get ononline() {
                return unwrap(this).ononline;
            },
            set ononline(newval) {
                unwrap(this).ononline = toCallbackOrNull(newval);
            },

            get onpagehide() {
                return unwrap(this).onpagehide;
            },
            set onpagehide(newval) {
                unwrap(this).onpagehide = toCallbackOrNull(newval);
            },

            get onpageshow() {
                return unwrap(this).onpageshow;
            },
            set onpageshow(newval) {
                unwrap(this).onpageshow = toCallbackOrNull(newval);
            },

            get onpopstate() {
                return unwrap(this).onpopstate;
            },
            set onpopstate(newval) {
                unwrap(this).onpopstate = toCallbackOrNull(newval);
            },

            get onresize() {
                return unwrap(this).onresize;
            },
            set onresize(newval) {
                unwrap(this).onresize = toCallbackOrNull(newval);
            },

            get onscroll() {
                return unwrap(this).onscroll;
            },
            set onscroll(newval) {
                unwrap(this).onscroll = toCallbackOrNull(newval);
            },

            get onstorage() {
                return unwrap(this).onstorage;
            },
            set onstorage(newval) {
                unwrap(this).onstorage = toCallbackOrNull(newval);
            },

            get onunload() {
                return unwrap(this).onunload;
            },
            set onunload(newval) {
                unwrap(this).onunload = toCallbackOrNull(newval);
            },

        },
    });
});

//
// Interface HTMLFrameElement
//

defineLazyProperty(global, "HTMLFrameElement", function() {
    return idl.HTMLFrameElement.publicInterface;
}, true);

defineLazyProperty(idl, "HTMLFrameElement", function() {
    return new IDLInterface({
        name: "HTMLFrameElement",
        superclass: idl.HTMLElement,
        members: {
            get name() {
                return unwrap(this).name;
            },
            set name(newval) {
                unwrap(this).name = String(newval);
            },

            get scrolling() {
                return unwrap(this).scrolling;
            },
            set scrolling(newval) {
                unwrap(this).scrolling = String(newval);
            },

            get src() {
                return unwrap(this).src;
            },
            set src(newval) {
                unwrap(this).src = String(newval);
            },

            get frameBorder() {
                return unwrap(this).frameBorder;
            },
            set frameBorder(newval) {
                unwrap(this).frameBorder = String(newval);
            },

            get longDesc() {
                return unwrap(this).longDesc;
            },
            set longDesc(newval) {
                unwrap(this).longDesc = String(newval);
            },

            get noResize() {
                return unwrap(this).noResize;
            },
            set noResize(newval) {
                unwrap(this).noResize = Boolean(newval);
            },

            get contentDocument() {
                return wrap(unwrap(this).contentDocument);
            },

            get contentWindow() {
                return wrap(unwrap(this).contentWindow);
            },

            get marginHeight() {
                return unwrap(this).marginHeight;
            },
            set marginHeight(newval) {
                unwrap(this).marginHeight = StringOrEmpty(newval);
            },

            get marginWidth() {
                return unwrap(this).marginWidth;
            },
            set marginWidth(newval) {
                unwrap(this).marginWidth = StringOrEmpty(newval);
            },

        },
    });
});



/************************************************************************
 *  src/windowobjs.js
 ************************************************************************/

//@line 1 "src/windowobjs.js"
//
// DO NOT EDIT.
// This file was generated by idl2domjs from src/idl/windowobjs.idl
//


//
// Interface Location
//

defineLazyProperty(global, "Location", function() {
    return idl.Location.publicInterface;
}, true);

defineLazyProperty(idl, "Location", function() {
    return new IDLInterface({
        name: "Location",
        members: {
            get href() {
                return unwrap(this).href;
            },
            set href(newval) {
                unwrap(this).href = String(newval);
            },

            toString: function toString() {
                return unwrap(this).toString();
            },

            assign: function assign(url) {
                unwrap(this).assign(String(url));
            },

            replace: function replace(url) {
                unwrap(this).replace(String(url));
            },

            reload: function reload() {
                unwrap(this).reload();
            },

            get protocol() {
                return unwrap(this).protocol;
            },
            set protocol(newval) {
                unwrap(this).protocol = String(newval);
            },

            get host() {
                return unwrap(this).host;
            },
            set host(newval) {
                unwrap(this).host = String(newval);
            },

            get hostname() {
                return unwrap(this).hostname;
            },
            set hostname(newval) {
                unwrap(this).hostname = String(newval);
            },

            get port() {
                return unwrap(this).port;
            },
            set port(newval) {
                unwrap(this).port = String(newval);
            },

            get pathname() {
                return unwrap(this).pathname;
            },
            set pathname(newval) {
                unwrap(this).pathname = String(newval);
            },

            get search() {
                return unwrap(this).search;
            },
            set search(newval) {
                unwrap(this).search = String(newval);
            },

            get hash() {
                return unwrap(this).hash;
            },
            set hash(newval) {
                unwrap(this).hash = String(newval);
            },

        },
    });
});

//
// Interface History
//

defineLazyProperty(global, "History", function() {
    return idl.History.publicInterface;
}, true);

defineLazyProperty(idl, "History", function() {
    return new IDLInterface({
        name: "History",
        members: {
            go: function go(delta) {
                unwrap(this).go(OptionaltoLong(delta));
            },

            back: function back() {
                unwrap(this).back();
            },

            forward: function forward() {
                unwrap(this).forward();
            },

        },
    });
});

//
// Interface Navigator
//

defineLazyProperty(global, "Navigator", function() {
    return idl.Navigator.publicInterface;
}, true);

defineLazyProperty(idl, "Navigator", function() {
    return new IDLInterface({
        name: "Navigator",
        members: {
            get appName() {
                return unwrap(this).appName;
            },

            get appVersion() {
                return unwrap(this).appVersion;
            },

            get platform() {
                return unwrap(this).platform;
            },

            get userAgent() {
                return unwrap(this).userAgent;
            },

        },
    });
});



/************************************************************************
 *  src/AttrArrayProxy.js
 ************************************************************************/

//@line 1 "src/AttrArrayProxy.js"
// XXX
// This class is mostly the same as ArrayProxy, but it uses the item() method
// of the Attributes object instead of directly indexing an array.  Maybe
// it could be merged into a more general Array-like Proxy type.
//

// A factory function for AttrArray proxy objects
function AttrArrayProxy(attributes) {
    // This function expects an AttributesArray object (see impl/Element.js)
    // Note, though that that object just has an element property that points
    // back to the Element object.  This proxy is based on the element object.
    var handler = O.create(AttrArrayProxy.handler);
    handler.element = attributes.element;
    handler.localprops = O.create(null);
    return Proxy.create(handler, Array.prototype);
}

// This is the prototype object for the proxy handler object
//
// For now, while the Proxy spec is still in flux, this handler
// defines only the fundamental traps.  We can add the derived traps
// later if there is a performance bottleneck.
AttrArrayProxy.handler = {
    isArrayIndex: function(name) { return String(toULong(name)) === name; },

    getOwnPropertyDescriptor: function getOwnPropertyDescriptor(name) {
        if (name === "length") {
            return {
                value: this.element._numattrs,
                writable: false,
                enumerable: false,
                configurable: true
            };
        }
        if (this.isArrayIndex(name)) {
            if (name < this.element._numattrs) {
                var v = this.element._attr(name);
                if (v) {
                    return {
                        value: wrap(v),
                        writable: false,
                        enumerable: true,
                        configurable: true
                    };
                }
            }
        }
        else {
            return O.getOwnPropertyDescriptor(this.localprops, name);
        }
    },
    getPropertyDescriptor: function(name) {
        var desc = this.getOwnPropertyDescriptor(name) ||
            O.getOwnPropertyDescriptor(A.prototype, name) ||
            O.getOwnPropertyDescriptor(O.prototype, name);
        if (desc) desc.configurable = true; // Proxies require this
        return desc;
    },
    getOwnPropertyNames: function getOwnPropertyNames() {
        var r = ["length"];
        for (var i = 0, n = this.element._numattrs; i < n; i++)
            push(r, String(i));
        return concat(r, O.getOwnPropertyNames(this.localprops));
    },
    defineProperty: function(name, desc) {
        // XXX
        // The WebIDL algorithm says we should "Reject" these attempts by
        // throwing or returning false, depending on the Throw argument, which
        // is usually strict-mode dependent.  While this is being clarified
        // I'll just throw here.  May need to change this to return false
        // instead.
        if (this.isArrayIndex(name) || name === "length")
            throw TypeError("read-only array");
        desc.configurable = true;
        O.defineProperty(this.localprops, name, desc);
    },
    delete: function(name) {
        // Can't delete the length property
        if (name === "length") return false;

        // Can't delete index properties
        if (this.isArrayIndex(name)) {
            // If an item exists at that index, return false: won't delete it
            // Otherwise, if no item, then the index was out of bounds and
            // we return true to indicate that the deletion was "successful"
            var idx = toULong(name);
            return idx >= this.element._numattrs;
        }
        return delete this.localprops[name];
    },

    // WebIDL: Host objects implementing an interface that supports
    // indexed or named properties defy being fixed; if Object.freeze,
    // Object.seal or Object.preventExtensions is called on one, these
    // the function MUST throw a TypeError.
    //
    // Proxy proposal: When handler.fix() returns undefined, the
    // corresponding call to Object.freeze, Object.seal, or
    // Object.preventExtensions will throw a TypeError.
    fix: function() {},

    // Get all enumerable properties
    // XXX: Remove this method when this bug is fixed:
    // https://bugzilla.mozilla.org/show_bug.cgi?id=665198
    enumerate: function() {
        var r = [];
        for (var i = 0, n = this.element._numattrs; i < n; i++)
            push(r, String(i));
        for(var name in this.localprops) push(r, name);
        for(var name in Array.prototype) push(r, name);
        return r;
    }
};



/************************************************************************
 *  src/NodeListProxy.js
 ************************************************************************/

//@line 1 "src/NodeListProxy.js"
// A factory function for NodeList proxy objects
function NodeListProxy(list) {
    // This function expects an object with a length property and an item()
    // method.  If we pass it a plain array, it will add the item() method
    //
    // We should avoid reading the length property of the list when possible
    // because in lazy implementations such as impl/FilteredElementList,
    // reading the length forces the filter to process the entire document
    // tree undoing the laziness.
    if (isArray(list)) {
        if (!hasOwnProperty(list, "item"))
            list.item = function(n) { return list[n]; };
    }

    var handler = O.create(NodeListProxy.handler);
    handler.list = list;
    handler.localprops = O.create(null);
    var p = Proxy.create(handler, idl.NodeList.prototype);

    return p;
}

// This is the prototype object for the proxy handler object
//
// For now, while the Proxy spec is still in flux, this handler
// defines only the fundamental traps.  We can add the derived traps
// later if there is a performance bottleneck.
NodeListProxy.handler = {
    isArrayIndex: function(name) { return String(toULong(name)) === name; },

    // try adding this to make Node proxies work right
    // Need to work around the "illegal access" error
/*
    get: function(receiver, name) {

        if (this.isArrayIndex(name)) {
            return this.list.item(name);
        }
        else if (name in this.localprops) {
            return this.localprops[name];
        }
        else {
            return idl.NodeList.prototype[name]
        }
    },
*/
    getOwnPropertyDescriptor: function getOwnPropertyDescriptor(name) {
        if (this.isArrayIndex(name)) {
            // If the index is greater than the length, then we'll just
            // get null or undefined here and do nothing. That is better
            // than testing length.
            var v = this.list.item(name);
            if (v) {
                return {
                    value: wrap(v, idl.Node),
                    writable: false,
                    enumerable: true,
                    configurable: true
                };
            }
            else {
                // We're never going to allow array index properties to be
                // set on localprops, so we don't have to do the test
                // below and can just return nothing now.
                return;
            }
        }
        return O.getOwnPropertyDescriptor(this.localprops, name);
    },
    getPropertyDescriptor: function(name) {
        var desc = this.getOwnPropertyDescriptor(name) ||
            O.getOwnPropertyDescriptor(idl.NodeList.prototype, name) ||
            O.getOwnPropertyDescriptor(O.prototype, name);
        if (desc) desc.configurable = true; // Proxies require this
        return desc;
    },
    getOwnPropertyNames: function getOwnPropertyNames() {
        var r = [];
        for (var i = 0, n = this.list.length; i < n; i++)
            push(r, String(i));
        return concat(r, O.getOwnPropertyNames(this.localprops));
    },
    defineProperty: function(name, desc) {
        // XXX
        // The WebIDL algorithm says we should "Reject" these attempts by
        // throwing or returning false, depending on the Throw argument, which
        // is usually strict-mode dependent.  While this is being clarified
        // I'll just throw here.  May need to change this to return false
        // instead.
        if (this.isArrayIndex(name))
            throw new TypeError(
                "can't set or create indexed properties '" + name + "'");

        O.defineProperty(this.localprops, name, desc);
    },
    delete: function(name) {
        // Can't delete index properties
        if (this.isArrayIndex(name)) {
            // If an item exists at that index, return false: won't delete it
            // Otherwise, if no item, then the index was out of bounds and
            // we return true to indicate that the deletion was "successful"
            return !this.list.item(name);
        }
        return delete this.localprops[name];
    },

    // WebIDL: Host objects implementing an interface that supporst
    // indexed or named properties defy being fixed; if Object.freeze,
    // Object.seal or Object.preventExtensions is called on one, these
    // the function MUST throw a TypeError.
    //
    // Proxy proposal: When handler.fix() returns undefined, the
    // corresponding call to Object.freeze, Object.seal, or
    // Object.preventExtensions will throw a TypeError.
    fix: function() {},

    // Get all enumerable properties
    // XXX: Remove this method when this bug is fixed:
    // https://bugzilla.mozilla.org/show_bug.cgi?id=665198
    enumerate: function() {
        var r = [];
        for (var i = 0, n = this.list.length; i < n; i++)
            push(r, String(i));
        for(var name in this.localprops) push(r, name);
        for(var name in idl.NodeList.prototype) push(r, name);
        return r;
    }
};



/************************************************************************
 *  src/HTMLCollectionProxy.js
 ************************************************************************/

//@line 1 "src/HTMLCollectionProxy.js"
// A factory function for HTMLCollection proxy objects.
// Expects an object with a length property and item() and namedItem() methods.
// That object must also have a namedItems property that returns an object
// that maps element names to some value.
//
// XXX: bug I can't define an expando property if there is a named property
// with the same name. I think it is a bug in the Proxy itself.  Looks like
// define property is not even being called.
//
function HTMLCollectionProxy(collection) {
    var handler = O.create(HTMLCollectionProxy.handler);
    handler.collection = collection;
    handler.localprops = O.create(null);
    return Proxy.create(handler, idl.HTMLCollection.prototype);
}

// This is the prototype object for the proxy handler object
HTMLCollectionProxy.handler = {
    isArrayIndex: function(name) { return String(toULong(name)) === name; },

    // This is the "named property visibility algorithm" from WebIDL
    isVisible: function(name) {
        // 1) If P is not a supported property name of O, then return false.
        if (!(name in this.collection.namedItems)) return false;

        // 2) If O implements an interface that has the
        // [OverrideBuiltins] extended attribute, then return true.
        // HTMLCollection does not OverrideBuiltins, so skip this step

        // 3) If O has an own property named P, then return false.
        if (hasOwnProperty(this.localprops, name)) return false;

        // 4) Let prototype be the value of the internal [[Prototype]]
        // property of O.
        // 5) If prototype is null, then return true.
        // 6) If the result of calling the [[HasProperty]] internal
        // method on prototype with property name P is true, then
        // return false.
        if (name in idl.HTMLCollection.prototype) return false;

        // 7) Return true.
        return true;
    },

    getOwnPropertyDescriptor: function getOwnPropertyDescriptor(name) {
        var item;
        if (this.isArrayIndex(name)) {
            var idx = toULong(name);
            if (idx < this.collection.length) {
                return {
                    value: wrap(this.collection.item(idx), idl.Element),
                    writable: false,
                    enumerable: true,
                    configurable: true
                };
            }
        }

        if (this.isVisible(name)) {
            return {
                value: wrap(this.collection.namedItem(name), idl.Element),
                writable: false,
                enumerable: true,
                configurable: true
            };
        }

        return O.getOwnPropertyDescriptor(this.localprops, name);
    },

    getPropertyDescriptor: function(name) {
        var desc = this.getOwnPropertyDescriptor(name) ||
            O.getOwnPropertyDescriptor(idl.HTMLCollection.prototype, name) ||
            O.getOwnPropertyDescriptor(Object.prototype, name);
        if (desc) desc.configurable = true; // Proxies require this
        return desc;
    },

    getOwnPropertyNames: function getOwnPropertyNames() {
        var names = [];
        for (var i = 0, n = this.collection.length; i < n; i++)
            push(names, String(i));
        for(var n in this.collection.namedItems)
            push(names, n);
        return concat(r, O.getOwnPropertyNames(this.localprops));
    },

    defineProperty: function(name, desc) {
        // XXX
        // For now, we "Reject" by throwing TypeError.  Proxies may change
        // so we only have to return false.
        if (this.isArrayIndex(name))
            throw new TypeError(
                "can't set or create indexed property '" + name + "'");

        // Don't allow named properties to overridden by expando properties,
        // even with an explicit Object.defineProperty() call.
        // XXX
        // The resolution of this issue is still pending on the mailing list.
        if (name in this.collection.namedItems)
            throw new TypeError(
                "can't override named property '" + name + "'");

        desc.configurable = true;
        O.defineProperty(this.localprops, name, desc);
    },

    delete: function(name) {
        // Can't delete array elements, but if they don't exist, don't complain
        if (this.isArrayIndex(name)) {
            var idx = toULong(name);
            return idx >= this.collection.length;
        }

        // Can't delete named properties
        if (this.isVisible(name)) {
            return false;
        }

        // Finally, try deleting an expando
        return delete this.localprops[name];
    },

    fix: function() {},

    // Get all enumerable properties
    // XXX: Remove this method when this bug is fixed:
    // https://bugzilla.mozilla.org/show_bug.cgi?id=665198
    enumerate: function() {
        var names = [];
        for (var i = 0, n = this.collection.length; i < n; i++)
            push(names, String(i));
        for(var n in this.collection.namedItems)
            push(names, n);
        for(var name in this.localprops)
            push(names, name);
        for(var name in idl.HTMLCollection.prototype)
            push(names, name);
        return names;
    }
};



/************************************************************************
 *  src/DOMException.js
 ************************************************************************/

//@line 1 "src/DOMException.js"
//
// This DOMException implementation is not WebIDL compatible.
// WebIDL exceptions are in flux right now, so I'm just doing something
// simple and approximately web compatible for now.
//
const INDEX_SIZE_ERR = 1;
const HIERARCHY_REQUEST_ERR = 3;
const WRONG_DOCUMENT_ERR = 4;
const INVALID_CHARACTER_ERR = 5;
const NO_MODIFICATION_ALLOWED_ERR = 7;
const NOT_FOUND_ERR = 8;
const NOT_SUPPORTED_ERR = 9;
const INVALID_STATE_ERR = 11;
const SYNTAX_ERR = 12;
const INVALID_MODIFICATION_ERR = 13;
const NAMESPACE_ERR = 14;
const INVALID_ACCESS_ERR = 15;
const TYPE_MISMATCH_ERR = 17;
const SECURITY_ERR = 18;
const NETWORK_ERR = 19;
const ABORT_ERR = 20;
const URL_MISMATCH_ERR = 21;
const QUOTA_EXCEEDED_ERR = 22;
const TIMEOUT_ERR = 23;
const INVALID_NODE_TYPE_ERR = 24;
const DATA_CLONE_ERR = 25;

global.DOMException = (function() {
    // Code to name
    const names = [
        null,  // No error with code 0
        "INDEX_SIZE_ERR",
        null, // historical
        "HIERARCHY_REQUEST_ERR",
        "WRONG_DOCUMENT_ERR",
        "INVALID_CHARACTER_ERR",
        null, // historical
        "NO_MODIFICATION_ALLOWED_ERR",
        "NOT_FOUND_ERR",
        "NOT_SUPPORTED_ERR",
        null, // historical
        "INVALID_STATE_ERR",
        "SYNTAX_ERR",
        "INVALID_MODIFICATION_ERR",
        "NAMESPACE_ERR",
        "INVALID_ACCESS_ERR",
        null, // historical
        "TYPE_MISMATCH_ERR",
        "SECURITY_ERR",
        "NETWORK_ERR",
        "ABORT_ERR",
        "URL_MISMATCH_ERR",
        "QUOTA_EXCEEDED_ERR",
        "TIMEOUT_ERR",
        "INVALID_NODE_TYPE_ERR",
        "DATA_CLONE_ERR",
    ];

    // Code to message
    // These strings are from the 13 May 2011 Editor's Draft of DOM Core.
    // http://dvcs.w3.org/hg/domcore/raw-file/tip/Overview.html
    // Copyright © 2011 W3C® (MIT, ERCIM, Keio), All Rights Reserved.
    // Used under the terms of the W3C Document License:
    // http://www.w3.org/Consortium/Legal/2002/copyright-documents-20021231
    const messages = [
        null,  // No error with code 0
        "INDEX_SIZE_ERR (1): the index is not in the allowed range",
        null,
        "HIERARCHY_REQUEST_ERR (3): the operation would yield an incorrect nodes model",
        "WRONG_DOCUMENT_ERR (4): the object is in the wrong Document, a call to importNode is required",
        "INVALID_CHARACTER_ERR (5): the string contains invalid characters",
        null,
        "NO_MODIFICATION_ALLOWED_ERR (7): the object can not be modified",
        "NOT_FOUND_ERR (8): the object can not be found here",
        "NOT_SUPPORTED_ERR (9): this operation is not supported",
        null,
        "INVALID_STATE_ERR (11): the object is in an invalid state",
        "SYNTAX_ERR (12): the string did not match the expected pattern",
        "INVALID_MODIFICATION_ERR (13): the object can not be modified in this way",
        "NAMESPACE_ERR (14): the operation is not allowed by Namespaces in XML",
        "INVALID_ACCESS_ERR (15): the object does not support the operation or argument",
        null,
        "TYPE_MISMATCH_ERR (17): the type of the object does not match the expected type",
        "SECURITY_ERR (18): the operation is insecure",
        "NETWORK_ERR (19): a network error occurred",
        "ABORT_ERR (20): the user aborted an operation",
        "URL_MISMATCH_ERR (21): the given URL does not match another URL",
        "QUOTA_EXCEEDED_ERR (22): the quota has been exceeded",
        "TIMEOUT_ERR (23): a timeout occurred",
        "INVALID_NODE_TYPE_ERR (24): the supplied node is invalid or has an invalid ancestor for this operation",
        "DATA_CLONE_ERR (25): the object can not be cloned.",
    ];

    // Name to code
    const constants = {
        INDEX_SIZE_ERR: INDEX_SIZE_ERR,
        DOMSTRING_SIZE_ERR: 2, // historical
        HIERARCHY_REQUEST_ERR: HIERARCHY_REQUEST_ERR,
        WRONG_DOCUMENT_ERR: WRONG_DOCUMENT_ERR,
        INVALID_CHARACTER_ERR: INVALID_CHARACTER_ERR,
        NO_DATA_ALLOWED_ERR: 6, // historical
        NO_MODIFICATION_ALLOWED_ERR: NO_MODIFICATION_ALLOWED_ERR,
        NOT_FOUND_ERR: NOT_FOUND_ERR,
        NOT_SUPPORTED_ERR: NOT_SUPPORTED_ERR,
        INUSE_ATTRIBUTE_ERR: 10, // historical
        INVALID_STATE_ERR: INVALID_STATE_ERR,
        SYNTAX_ERR: SYNTAX_ERR,
        INVALID_MODIFICATION_ERR: INVALID_MODIFICATION_ERR,
        NAMESPACE_ERR: NAMESPACE_ERR,
        INVALID_ACCESS_ERR: INVALID_ACCESS_ERR,
        VALIDATION_ERR: 16, // historical
        TYPE_MISMATCH_ERR: TYPE_MISMATCH_ERR,
        SECURITY_ERR: SECURITY_ERR,
        NETWORK_ERR: NETWORK_ERR,
        ABORT_ERR: ABORT_ERR,
        URL_MISMATCH_ERR: URL_MISMATCH_ERR,
        QUOTA_EXCEEDED_ERR: QUOTA_EXCEEDED_ERR,
        TIMEOUT_ERR: TIMEOUT_ERR,
        INVALID_NODE_TYPE_ERR: INVALID_NODE_TYPE_ERR,
        DATA_CLONE_ERR: DATA_CLONE_ERR,
    };

    function DOMException(code) {
/*
        // This kudge is so we get lineNumber, fileName and stack properties
        var e = Error(messages[code]);
        e.__proto__ = DOMException.prototype;
*/

        var e = O.create(DOMException.prototype);
        e.code = code;
        e.message = messages[code];
        e.name = names[code];

        // Get stack, lineNumber and fileName properties like a real
        // Error object has.
        var x = Error();
        var frames = split(x.stack,"\n");
        A.shift(frames);
        e.stack = join(frames,"\n");
        var parts = match(frames[0], /[^@]*[@(]([^:]*):(\d*)/);

        if (parts) {
            e.fileName = parts[1];
            e.lineNumber = parts[2];
        }
        else {
            e.fileName = "";
            e.lineNumber = -1;
        }

        return e;
    }

    DOMException.prototype = O.create(Error.prototype);

    // Initialize the constants on DOMException and DOMException.prototype
    for(var c in constants) {
        var v = constants[c];
        defineConstantProp(DOMException, c, v);
        defineConstantProp(DOMException.prototype, c, v);
    }

    return DOMException;
}());

//
// Shortcut functions for throwing errors of various types.
//
function IndexSizeError() { throw DOMException(INDEX_SIZE_ERR); }
function HierarchyRequestError() { throw DOMException(HIERARCHY_REQUEST_ERR); }
function WrongDocumentError() { throw DOMException(WRONG_DOCUMENT_ERR); }
function InvalidCharacterError() { throw DOMException(INVALID_CHARACTER_ERR); }
function NoModificationAllowedError() { throw DOMException(NO_MODIFICATION_ALLOWED_ERR); }
function NotFoundError() { throw DOMException(NOT_FOUND_ERR); }
function NotSupportedError() { throw DOMException(NOT_SUPPORTED_ERR); }
function InvalidStateError() { throw DOMException(INVALID_STATE_ERR); }
function SyntaxError() { throw DOMException(SYNTAX_ERR); }
function InvalidModificationError() { throw DOMException(INVALID_MODIFICATION_ERR); }
function NamespaceError() { throw DOMException(NAMESPACE_ERR); }
function InvalidAccessError() { throw DOMException(INVALID_ACCESS_ERR); }
function TypeMismatchError() { throw DOMException(TYPE_MISMATCH_ERR); }
function SecurityError() { throw DOMException(SECURITY_ERR); }
function NetworkError() { throw DOMException(NETWORK_ERR); }
function AbortError() { throw DOMException(ABORT_ERR); }
function UrlMismatchError() { throw DOMException(URL_MISMATCH_ERR); }
function QuotaExceededError() { throw DOMException(QUOTA_EXCEEDED_ERR); }
function TimeoutError() { throw DOMException(TIMEOUT_ERR); }
function InvalidNodeTypeError() { throw DOMException(INVALID_NODE_TYPE_ERR); }
function DataCloneError() { throw DOMException(DATA_CLONE_ERR); }




/************************************************************************
 *  src/impl/EventTarget.js
 ************************************************************************/

//@line 1 "src/impl/EventTarget.js"
defineLazyProperty(impl, "EventTarget", function() {
    function EventTarget() {}

    EventTarget.prototype = {
        _idlName: "EventTarget",

        // XXX
        // See WebIDL §4.8 for details on object event handlers
        // and how they should behave.  We actually have to accept
        // any object to addEventListener... Can't type check it.
        // on registration.

        // XXX:
        // Capturing event listeners are sort of rare.  I think I can optimize
        // them so that dispatchEvent can skip the capturing phase (or much of
        // it).  Each time a capturing listener is added, increment a flag on
        // the target node and each of its ancestors.  Decrement when removed.
        // And update the counter when nodes are added and removed from the
        // tree as well.  Then, in dispatch event, the capturing phase can
        // abort if it sees any node with a zero count.
        addEventListener: function addEventListener(type, listener, capture) {
            if (!listener) return;
            if (capture === undefined) capture = false;
            if (!this._listeners) this._listeners = {};
            if (!(type in this._listeners)) this._listeners[type] = {};
            var list = this._listeners[type];

            // If this listener has already been registered, just return
            for(var i = 0, n = list.length; i < n; i++) {
                var l = list[i];
                if (l.listener === listener && l.capture === capture)
                    return;
            }

            // Add an object to the list of listeners
            var obj = { listener: listener, capture: capture };
            if (typeof listener === "function") obj.f = listener;
            push(list, obj);
        },

        removeEventListener: function removeEventListener(type,
                                                          listener,
                                                          capture) {
            if (capture === undefined) capture = false;
            if (this._listeners) {
                var list = this._listeners[type];
                if (list) {
                    // Find the listener in the list and remove it
                    for(var i = 0, n = list.length; i < n; i++) {
                        var l = list[i];
                        if (l.listener === listener && l.capture === capture) {
                            if (list.length === 1)
                                delete this._listeners[type];
                            else
                                splice(list, i, 1);
                        }
                    }
                }
            }
        },

        // This is the public API for dispatching untrusted public events.
        // See _dispatchEvent for the implementation
        dispatchEvent: function dispatchEvent(event) {
            // Dispatch an untrusted event
            return this._dispatchEvent(event, false);
        },

        //
        // See DOMCore §4.4
        // XXX: I'll probably need another version of this method for
        // internal use, one that does not set isTrusted to false.
        // XXX: see Document._dispatchEvent: perhaps that and this could
        // call a common internal function with different settings of
        // a trusted boolean argument
        //
        // XXX:
        // The spec has changed in how to deal with handlers registered
        // on idl or content attributes rather than with addEventListener.
        // Used to say that they always ran first.  That's how webkit does it
        // Spec now says that they run in a position determined by
        // when they were first set.  FF does it that way.  See:
        // http://www.whatwg.org/specs/web-apps/current-work/multipage/webappapis.html#event-handlers
        //
        _dispatchEvent: function _dispatchEvent(event, trusted) {
            if (typeof trusted !== "boolean") trusted = false;
            function invoke(target, event) {
                var type = event.type, phase = event.eventPhase;
                event.currentTarget = target;

                // If there was an individual handler defined, invoke it first
                // XXX: see comment above: this shouldn't always be first.
                if (phase !== CAPTURING_PHASE &&
                    target._handlers && target._handlers[type])
                {
                    var handler = target._handlers[type];
                    var rv;
                    if (typeof handler === "function") {
                        rv=handler.call(wrap(event.currentTarget), wrap(event));
                    }
                    else {
                        var f = handler.handleEvent;
                        if (typeof f !== "function")
                            throw TypeError("handleEvent property of " +
                                            "event handler object is" +
                                            "not a function.");
                        rv=f.call(handler, wrap(event));
                    }

                    switch(event.type) {
                    case "mouseover":
                        if (rv === true)  // Historical baggage
                            event.preventDefault();
                        break;
                    case "beforeunload":
                        // XXX: eventually we need a special case here
                    default:
                        if (rv === false)
                            event.preventDefault();
                        break;
                    }
                }

                // Now invoke list list of listeners for this target and type
                var list = target._listeners && target._listeners[type];
                if (!list) return;

                for(var i = 0, n = list.length; i < n; i++) {
                    if (event._stopImmediatePropagation) return;
                    var l = list[i];
                    if ((phase === CAPTURING_PHASE && !l.capture) ||
                        (phase === BUBBLING_PHASE && l.capture))
                        continue;
                    if (l.f) {
                        // Wrap both the this value of the call and the
                        // argument passed to the call, since these objects
                        // impl are being exposed through the public API
                        l.f.call(wrap(event.currentTarget), wrap(event));
                    }
                    else {
                        var f = l.listener.handleEvent;
                        if (typeof f !== "function")
                            throw TypeError("handleEvent property of " +
                                            "event listener object is " +
                                            "not a function.");
                        // Here we only have to wrap the event object, since
                        // the listener object was passed in to us from
                        // the public API.
                        f.call(l.listener, wrap(event));
                    }
                }
            }

            if (!event._initialized || event._dispatching) InvalidStateError();
            event.isTrusted = trusted;

            // Begin dispatching the event now
            event._dispatching = true;
            event.target = this;

            // Build the list of targets for the capturing and bubbling phases
            // XXX: we'll eventually have to add Window to this list.
            var ancestors = [];
            for(var n = this.parentNode; n; n = n.parentNode)
                push(ancestors, n);

            // Capturing phase
            event.eventPhase = CAPTURING_PHASE;
            for(var i = ancestors.length-1; i >= 0; i--) {
                invoke(ancestors[i], event);
                if (event._propagationStopped) break;
            }

            // At target phase
            if (!event._propagationStopped) {
                event.eventPhase = AT_TARGET;
                invoke(this, event);
            }

            // Bubbling phase
            if (event.bubbles && !event._propagationStopped) {
                event.eventPhase = BUBBLING_PHASE;
                for(var i = 0, n = ancestors.length; i < n; i++) {
                    invoke(ancestors[i], event);
                    if (event._propagationStopped) break;
                }
            }

            event._dispatching = false;
            event.eventPhase = AT_TARGET;
            event.currentTarget = null;

            // Deal with mouse events and figure out when
            // a click has happened
            if (trusted &&
                !event.defaultPrevented &&
                event instanceof impl.MouseEvent)
            {
                switch(event.type) {
                case "mousedown":
                    this._armed = {
                        x: event.clientX,
                        y: event.clientY,
                        t: event.timeStamp
                    };
                    break;
                case "mouseout":
                case "mouseover":
                    this._armed = null;
                    break;
                case "mouseup":
                    if (this._isClick(event)) this._doClick(event);
                    this._armed = null;
                    break;
                }
            }



            return !event.defaultPrevented;
        },

        // Determine whether a click occurred
        // XXX We don't support double clicks for now
        _isClick: function(event) {
            return (this._armed !== null &&
                    event.type === "mouseup" &&
                    event.isTrusted &&
                    event.button === 0 &&
                    event.timeStamp - this._armed.t < 1000 &&
                    Math.abs(event.clientX - this._armed.x) < 10 &&
                    Math.abs(event.clientY - this._armed.Y) < 10);
        },

        // Clicks are handled like this:
        // http://www.whatwg.org/specs/web-apps/current-work/multipage/elements.html#interactive-content-0
        //
        // Note that this method is similar to the HTMLElement.click() method
        // The event argument must be the trusted mouseup event
        _doClick: function(event) {
            if (this._click_in_progress) return;
            this._click_in_progress = true;

            // Find the nearest enclosing element that is activatable
            // An element is activatable if it has a
            // _post_click_activation_steps hook
            var activated = this;
            while(activated && !activated._post_click_activation_steps)
                activated = activated.parentNode;

            if (activated && activated._pre_click_activation_steps) {
                activated._pre_click_activation_steps();
            }

            var click = this.ownerDocument.createEvent("MouseEvent");
            click.initMouseEvent("click", true, true,
                                 this.ownerDocument.defaultView, 1,
                                 event.screenX, event.screenY,
                                 event.clientX, event.clientY,
                                 event.ctrlKey, event.altKey,
                                 event.shiftKey, event.metaKey,
                                 event.button, null);

            var result = this._dispatchEvent(click, true);

            if (activated) {
                if (result) {
                    // This is where hyperlinks get followed, for example.
                    if (activated._post_click_activation_steps)
                        activated._post_click_activation_steps(click);
                }
                else {
                    if (activated._cancelled_activation_steps)
                        activated._cancelled_activation_steps();
                }
            }
        },

        //
        // An event handler is like an event listener, but it registered
        // by setting an IDL or content attribute like onload or onclick.
        // There can only be one of these at a time for any event type.
        // This is an internal method for the attribute accessors and
        // content attribute handlers that need to register events handlers.
        // The type argument is the same as in addEventListener().
        // The handler argument is the same as listeners in addEventListener:
        // it can be a function or an object. Pass null to remove any existing
        // handler.  Handlers are always invoked before any listeners of
        // the same type.  They are not invoked during the capturing phase
        // of event dispatch.
        //
        _setEventHandler: function _setEventHandler(type, handler) {
            if (!this._handlers) this._handlers = {};

            if (handler)
                this._handlers[type] = handler;
            else
                delete this._handlers[type];
        },

        _getEventHandler: function _getEventHandler(type) {
            return (this._handlers && this._handlers[type]) || null;
        }

    };

    return EventTarget;
});


/************************************************************************
 *  src/impl/Node.js
 ************************************************************************/

//@line 1 "src/impl/Node.js"
defineLazyProperty(impl, "Node", function() {
    // All nodes have a nodeType and an ownerDocument.
    // Once inserted, they also have a parentNode.
    // This is an abstract class; all nodes in a document are instances
    // of a subtype, so all the properties are defined by more specific
    // constructors.
    function Node() {
    }

    Node.prototype = O.create(impl.EventTarget.prototype, {

        // Node that are not inserted into the tree inherit a null parent
        // XXX
        // Can't use constant(null) here because then I couldn't set a non-null
        // value that would override the inherited constant.  Perhaps that
        // means I shouldn't use the prototype and should just set the
        // value in each node constructor?
        parentNode: { value: null, writable: true },

        // XXX: the baseURI attribute is defined by dom core, but
        // a correct implementation of it requires HTML features, so
        // we'll come back to this later.
        baseURI: attribute(nyi),

        parentElement: attribute(function() {
            return (this.parentNode && this.parentNode.nodeType===ELEMENT_NODE)
                ? this.parentNode
                : null
        }),

        hasChildNodes: constant(function() {  // Overridden in leaf.js
            return this.childNodes.length > 0;
        }),

        firstChild: attribute(function() {
            return this.childNodes.length === 0
                ? null
                : this.childNodes[0];
        }),

        lastChild: attribute(function() {
            return this.childNodes.length === 0
                ? null
                : this.childNodes[this.childNodes.length-1];
        }),

        previousSibling: attribute(function() {
            if (!this.parentNode) return null;
            var sibs = this.parentNode.childNodes, i = this.index;
            return i === 0
                ? null
                : sibs[i-1]
        }),

        nextSibling: attribute(function() {
            if (!this.parentNode) return null;
            var sibs = this.parentNode.childNodes, i = this.index;
            return i+1 === sibs.length
                ? null
                : sibs[i+1]
        }),

        insertBefore: constant(function insertBefore(child, refChild) {
            var parent = this;
            if (refChild === null) return this.appendChild(child);
            if (refChild.parentNode !== parent) NotFoundError();
            if (child.isAncestor(parent)) HierarchyRequestError();
            if (child.nodeType === DOCUMENT_NODE) HierarchyRequestError();
            parent.ensureSameDoc(child);
            child.insert(parent, refChild.index);
            return child;
        }),


        appendChild: constant(function(child) {
            var parent = this;
            if (child.isAncestor(parent)) HierarchyRequestError();
            if (child.nodeType === DOCUMENT_NODE) HierarchyRequestError();
            parent.ensureSameDoc(child);
            child.insert(parent, parent.childNodes.length);
            return child;
        }),

        removeChild: constant(function removeChild(child) {
            var parent = this;
            if (child.parentNode !== parent) NotFoundError();
            child.remove();
            return child;
        }),

        replaceChild: constant(function replaceChild(newChild, oldChild) {
            var parent = this;
            if (oldChild.parentNode !== parent) NotFoundError();
            if (newChild.isAncestor(parent)) HierarchyRequestError();
            parent.ensureSameDoc(newChild);

            var refChild = oldChild.nextSibling;
            oldChild.remove();
            parent.insertBefore(newChild, refChild);
            return oldChild;
        }),

        compareDocumentPosition:constant(function compareDocumentPosition(that){
            // Basic algorithm for finding the relative position of two nodes.
            // Make a list the ancestors of each node, starting with the
            // document element and proceeding down to the nodes themselves.
            // Then, loop through the lists, looking for the first element
            // that differs.  The order of those two elements give the
            // order of their descendant nodes.  Or, if one list is a prefix
            // of the other one, then that node contains the other.

            if (this === that) return 0;

            // If they're not owned by the same document or if one is rooted
            // and one is not, then they're disconnected.
            if (this.ownerDocument != that.ownerDocument ||
                this.rooted !== that.rooted)
                return (DOCUMENT_POSITION_DISCONNECTED +
                        DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC);

            // Get arrays of ancestors for this and that
            var these = [], those = [];
            for(var n = this; n !== null; n = n.parentNode) push(these, n);
            for(var n = that; n !== null; n = n.parentNode) push(those, n);
            these.reverse();  // So we start with the outermost
            those.reverse();

            if (these[0] !== those[0]) // No common ancestor
                return (DOCUMENT_POSITION_DISCONNECTED +
                        DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC);

            var n = Math.min(these.length, those.length);
            for(var i = 1; i < n; i++) {
                if (these[i] !== those[i]) {
                    // We found two different ancestors, so compare
                    // their positions
                    if (these[i].index < those[i].index)
                        return DOCUMENT_POSITION_FOLLOWING;
                    else
                        return DOCUMENT_POSITION_PRECEDING;
                }
            }

            // If we get to here, then one of the nodes (the one with the
            // shorter list of ancestors) contains the other one.
            if (these.length < those.length)
                return (DOCUMENT_POSITION_FOLLOWING +
                        DOCUMENT_POSITION_CONTAINED_BY);
            else
                return (DOCUMENT_POSITION_PRECEDING +
                        DOCUMENT_POSITION_CONTAINS);
        }),

        isSameNode: constant(function isSameNode(node) {
            return this === node;
        }),


        // This method implements the generic parts of node equality testing
        // and defers to the (non-recursive) type-specific isEqual() method
        // defined by subclasses
        isEqualNode: constant(function isEqualNode(node) {
            if (!node) return false;
            if (node.nodeType !== this.nodeType) return false;

            // Check for same number of children
            // Check for children this way because it is more efficient
            // for childless leaf nodes.
            var n; // number of child nodes
            if (!this.firstChild) {
                n = 0;
                if (node.firstChild) return false;
            }
            else {
                n = this.childNodes.length;
                if (node.childNodes.length != n) return false;
            }

            // Check type-specific properties for equality
            if (!this.isEqual(node)) return false;

            // Now check children for equality
            for(var i = 0; i < n; i++) {
                var c1 = this.childNodes[i], c2 = node.childNodes[i];
                if (!c1.isEqualNode(c2)) return false;
            }

            return true;
        }),

        // This method delegates shallow cloning to a clone() method
        // that each concrete subclass must implement
        cloneNode: constant(function(deep) {
            // Clone this node
            var clone = this.clone();

            // Handle the recursive case if necessary
            if (deep && this.firstChild) {
                for(var i = 0, n = this.childNodes.length; i < n; i++) {
                    clone.appendChild(this.childNodes[i].cloneNode(true));
                }
            }

            return clone;
        }),

        lookupPrefix: constant(function lookupPrefix(ns) {
            var e;
            if (ns === "") return null;
            switch(this.nodeType) {
            case ELEMENT_NODE:
                return this.locateNamespacePrefix(ns);
            case DOCUMENT_NODE:
                e = this.documentElement;
                return e ? e.locateNamespacePrefix(ns) : null;
            case DOCUMENT_TYPE_NODE:
            case DOCUMENT_FRAGMENT_NODE:
                return null;
            default:
                e = this.parentElement;
                return e ? e.locateNamespacePrefix(ns) : null;
            }
        }),


        lookupNamespaceURI: constant(function lookupNamespaceURI(prefix) {
            var e;
            switch(this.nodeType) {
            case ELEMENT_NODE:
                return this.locateNamespace(prefix);
            case DOCUMENT_NODE:
                e = this.documentElement;
                return e ? e.locateNamespace(prefix) : null;
            case DOCUMENT_TYPE_NODE:
            case DOCUMENT_FRAGMENT_NODE:
                return null;
            default:
                e = this.parentElement;
                return e ? e.locateNamespace(prefix) : null;
            }
        }),

        isDefaultNamespace: constant(function isDefaultNamespace(ns) {
            var defaultns = this.lookupNamespaceURI(null);
            if (defaultns == null) defaultns = "";
            return ns === defaultns;
        }),

        // Utility methods for nodes.  Not part of the DOM

        // Return the index of this node in its parent.
        // Throw if no parent, or if this node is not a child of its parent
        index: attribute(function() {
            assert(this.parentNode);
            var kids = this.parentNode.childNodes
            if (this._index == undefined || kids[this._index] != this) {
                this._index = A.indexOf(kids, this);
                assert(this._index != -1);
            }
            return this._index;
        }),

        // Return true if this node is equal to or is an ancestor of that node
        // Note that nodes are considered to be ancestors of themselves
        isAncestor: constant(function(that) {
            // If they belong to different documents, then they're unrelated.
            if (this.ownerDocument != that.ownerDocument) return false;
            // If one is rooted and one isn't then they're not related
            if (this.rooted !== that.rooted) return false;

            // Otherwise check by traversing the parentNode chain
            for(var e = that; e; e = e.parentNode) {
                if (e === this) return true;
            }
            return false;
        }),

        // When a user agent is to ensure that two Nodes, old and new, are
        // in the same Document, it must run the following steps:
        //
        //     If new is a DocumentType, run the following steps:
        //
        //         If new's ownerDocument is not null, and it is not equal
        //         to old's ownerDocument, throw a WRONG_DOCUMENT_ERR
        //         exception and terminate these steps.
        //
        //         Otherwise, set its ownerDocument to old's
        //         ownerDocument.
        //
        //     Otherwise, invoke old's ownerDocument's adoptNode method
        //     with new as its argument.
        //
        //     If old's ownerDocument and new's ownerDocument are not the
        //     same, throw a HIERARCHY_REQUEST_ERR
        ensureSameDoc: constant(function(that) {
            // Get the owner of the node, the node itself, if it is a document
            var ownerdoc = this.ownerDocument || this;

            if (that.nodeType === DOCUMENT_TYPE_NODE) {
                if (that.ownerDocument !== null && that.ownerDocument !== ownerdoc)
                    WrongDocumentError();
                that.ownerDocument = ownerdoc;
            }
            else {
                // The spec's algorithm says to call adoptNode
                // unconditionally, which will remove it from its current
                // location in the document even it if is not changing
                // documents.  I don't do that here because that would cause a
                // lot of unnecessary uproot and reroot mutation events.
                if (that.ownerDocument !== ownerdoc)
                    ownerdoc.adoptNode(that);
            }

            // XXX: this step does not seem necessary.
            // If mutation events are added, however, it becomes necessary
            if (that.ownerDocument !== ownerdoc) HierarchyRequestError();
        }),

        // Remove this node from its parent
        remove: constant(function remove() {
            // Remove this node from its parents array of children
            splice(this.parentNode.childNodes, this.index, 1);

            // Update the structure id for all ancestors
            this.parentNode.modify();

            // Forget this node's parent
            delete this.parentNode;

            // Send mutation events if necessary
            if (this.rooted) this.ownerDocument.mutateRemove(this);
        }),

        // Remove all of this node's children.  This is a minor
        // optimization that only calls modify() once.
        removeChildren: constant(function removeChildren() {
            var root = this.rooted ? this.ownerDocument : null;
            for(var i = 0, n = this.childNodes.length; i < n; i++) {
                delete this.childNodes[i].parentNode;
                if (root) root.mutateRemove(this.childNodes[i]);
            }
            this.childNodes.length = 0; // Forget all children
            this.modify();              // Update last modified type once only
        }),

        // Insert this node as a child of parent at the specified index,
        // firing mutation events as necessary
        insert: constant(function insert(parent, index) {
            var child = this, kids = parent.childNodes;

            // If we are already a child of the specified parent, then t
            // the index may have to be adjusted.
            if (child.parentNode === parent) {
                var currentIndex = child.index;
                // If we're not moving the node, we're done now
                // XXX: or do DOM mutation events still have to be fired?
                if (currentIndex === index) return;

                // If the child is before the spot it is to be inserted at,
                // then when it is removed, the index of that spot will be
                // reduced.
                if (currentIndex < index) index--;
            }

            // Special case for document fragments
            // XXX: it is not at all clear that I'm handling this correctly.
            // Scripts should never get to see partially
            // inserted fragments, I think.  See:
            // http://lists.w3.org/Archives/Public/www-dom/2011OctDec/0130.html
            if (child.nodeType === DOCUMENT_FRAGMENT_NODE) {
                var  c;
                while(c = child.firstChild)
                    c.insert(parent, index++);
                return;
            }

            // If both the child and the parent are rooted, then we want to
            // transplant the child without uprooting and rerooting it.
            if (child.rooted && parent.rooted) {
                // Remove the child from its current position in the tree
                // without calling remove(), since we don't want to uproot it.
                var curpar = child.parentNode, curidx = child.index;
                splice(child.parentNode.childNodes, child.index, 1);
                curpar.modify();

                // And insert it as a child of its new parent
                child.parentNode = parent;
                splice(kids, index, 0, child);
                child._index = index;              // Optimization
                parent.modify();

                // Generate a move mutation event
                parent.ownerDocument.mutateMove(child);
            }
            else {
                // If the child already has a parent, it needs to be
                // removed from that parent, which may also uproot it
                if (child.parentNode) child.remove();

                // Now insert the child into the parent's array of children
                child.parentNode = parent;
                splice(kids, index, 0, child);
                parent.modify();
                child._index = index;              // Optimization

                // And root the child if necessary
                if (parent.rooted) parent.ownerDocument.mutateInsert(child);
            }

            // Script tags use this hook
            if (parent._addchildhook) parent._addchildhook(this);
        }),


        // Return the lastModTime value for this node. (For use as a
        // cache invalidation mechanism. If the node does not already
        // have one, initialize it from the owner document's modclock
        // property.  (Note that modclock does not return the actual
        // time; it is simply a counter incremented on each document
        // modification)
        lastModTime: attribute(function() {
            if (!this._lastModTime) {
                this._lastModTime = this.doc.modclock;
            }

            return this._lastModTime;
        }),

        // Increment the owner document's modclock and use the new
        // value to update the lastModTime value for this node and
        // all of its ancestors.  Nodes that have never had their
        // lastModTime value queried do not need to have a
        // lastModTime property set on them since there is no
        // previously queried value to ever compare the new value
        // against, so only update nodes that already have a
        // _lastModTime property.
        modify: constant(function() {
            var time = ++this.doc.modclock;
            for(var n = this; n; n = n.parentElement) {
                if (n._lastModTime) {
                    n._lastModTime = time;
                }
            }
        }),

        // This attribute is not part of the DOM but is quite helpful.
        // It returns the document with which a node is associated.  Usually
        // this is the ownerDocument. But ownerDocument is null for the
        // document object itself, so this is a handy way to get the document
        // regardless of the node type
        doc: attribute(function() {
            return this.ownerDocument || this;
        }),


        // If the node has a nid (node id), then it is rooted in a document
        rooted: attribute(function() {
            return !!this._nid;
        }),


        // Convert the children of a node to an HTML string.
        // This is used by the innerHTML getter
        // The serialization spec is at:
        // http://www.whatwg.org/specs/web-apps/current-work/multipage/the-end.html#serializing-html-fragments
        serialize: constant(function() {
            var s = "";
            for(var i = 0, n = this.childNodes.length; i < n; i++) {
                var kid = this.childNodes[i];
                switch(kid.nodeType) {
                case COMMENT_NODE:
                    s += "<!--" + kid.data + "-->";
                    break;
                case PROCESSING_INSTRUCTION_NODE:
                    s += "<?" + kid.target + " " + kid.data + ">";
                    break;
                case DOCUMENT_TYPE_NODE:
                    s += "<!DOCTYPE " + kid.name + ">";
                    break;
                case TEXT_NODE:
                case CDATA_SECTION_NODE:
                    var parenttag;
                    if (this.nodeType === ELEMENT_NODE &&
                        this.namespaceURI === HTML_NAMESPACE)
                        parenttag = this.tagName;
                    else
                        parenttag = "";
                    switch(parenttag) {
                    case "STYLE":
                    case "SCRIPT":
                    case "XMP":
                    case "IFRAME":
                    case "NOEMBED":
                    case "NOFRAMES":
                    case "PLAINTEXT":
                    case "NOSCRIPT":
                        s += kid.data;
                        break;
                    default:
                        s += escape(kid.data);
                        break;
                    }
                    break;
                case ELEMENT_NODE:
                    serializeElement(kid);
                    break;
                default:
                    InvalidStateError();
                }
            }

            return s;

            function serializeElement(kid) {
                var html = false, tagname;
                switch(kid.namespaceURI) {
                case HTML_NAMESPACE:
                    html = true;
                    /* fallthrough */
                case SVG_NAMESPACE:
                case MATHML_NAMESPACE:
                    tagname = kid.localName;
                    break;
                default:
                    tagname = kid.tagName;
                }

                s += '<' + tagname;

                for(var i = 0, n = kid._numattrs; i < n; i++) {
                    var a = kid._attr(i);
                    s += ' ' + attrname(a) + '="' + escapeAttr(a.value) + '"';
                }
                s += '>';

                var htmltag = html?tagname:"";
                switch(htmltag) {

                case "area":
                case "base":
                case "basefont":
                case "bgsound":
                case "br":
                case "col":
                case "command":
                case "embed":
                case "frame":
                case "hr":
                case "img":
                case "input":
                case "keygen":
                case "link":
                case "meta":
                case "param":
                case "source":
                case "track":
                case "wbr":
                    return;  // These can't have kids, so we're done

                case 'pre':
                case 'textarea':
                case 'listing':
                    s += "\n"; // Extra newline for these
                    /* fallthrough */
                default:
                    // Serialize children and add end tag for all others
                    s += kid.serialize();
                    s += "</" + tagname + ">";
                }
            }

            function escape(s) {
                return s.replace(/[&<>\u00A0]/g, function(c) {
                    switch(c) {
                    case "&": return "&amp;";
                    case "<": return "&lt;";
                    case ">": return "&gt;";
                    case "\u00A0": return "&nbsp;";
                    }
                });
            }

            function escapeAttr(s) {
                return s.replace(/[&"\u00A0]/g, function(c) {
                    switch(c) {
                    case '&': return "&amp;";
                    case '"': return "&quot;";
                    case '\u00A0': return "&nbsp;";
                    }
                });
            }

            function attrname(a) {
                switch(a.namespaceURI) {
                case null: return a.localName;
                case XML_NAMESPACE: return "xml:" + a.localName;
                case XLINK_NAMESPACE: return "xlink:" + a.localName;
                case XMLNS_NAMESPACE:
                    if (a.localName === "xmlns") return "xmlns";
                    else return "xmlns:" + a.localName;
                default:
                    return a.name;
                }
            }
        }),

    });

    return Node;
});


/************************************************************************
 *  src/impl/Leaf.js
 ************************************************************************/

//@line 1 "src/impl/Leaf.js"
defineLazyProperty(impl, "Leaf", function() {
    // This class defines common functionality for node subtypes that
    // can never have children
    function Leaf() {}

    Leaf.prototype = O.create(impl.Node.prototype, {
        hasChildNodes: constant(function() { return false; }),
        firstChild: constant(null),
        lastChild: constant(null),
        insertBefore: constant(HierarchyRequestError),
        replaceChild: constant(HierarchyRequestError),
        removeChild: constant(HierarchyRequestError),
        appendChild: constant(HierarchyRequestError),

        // Each node must have its own unique childNodes array.  But
        // leaves always have an empty array, so initialize it lazily.
        // If the childNodes property is read, we'll return an array
        // and define a read-only property directly on the object that
        // will shadow this one. I'd like to freeze the array, too, since
        // leaf nodes can never have children, but I'll end up having to add
        // a property to refer back to the IDL NodeList wrapper.
        childNodes: attribute(function() {
            var a = [];
            a._idlName = "NodeList";
            O.defineProperty(this, "childNodes", constant(a));
            return a;
        }),
    });

    return Leaf;
});


/************************************************************************
 *  src/impl/CharacterData.js
 ************************************************************************/

//@line 1 "src/impl/CharacterData.js"
defineLazyProperty(impl, "CharacterData", function() {
    function CharacterData() {
    }

    CharacterData.prototype = O.create(impl.Leaf.prototype, {
        _idlName: constant("CharacterData"),

        // DOMString substringData(unsigned long offset,
        //                         unsigned long count);
        // The substringData(offset, count) method must run these steps:
        //
        //     If offset is greater than the context object's
        //     length, throw an INDEX_SIZE_ERR exception and
        //     terminate these steps.
        //
        //     If offset+count is greater than the context
        //     object's length, return a DOMString whose value is
        //     the UTF-16 code units from the offsetth UTF-16 code
        //     unit to the end of data.
        //
        //     Return a DOMString whose value is the UTF-16 code
        //     units from the offsetth UTF-16 code unit to the
        //     offset+countth UTF-16 code unit in data.
        substringData: constant(function substringData(offset, count) {
            if (offset > this.data.length) IndexSizeError();
            return substring(this.data, offset, offset+count);
        }),

        // void appendData(DOMString data);
        // The appendData(data) method must append data to the context
        // object's data.
        appendData: constant(function appendData(data) {
            this.data = this.data + data;
        }),

        // void insertData(unsigned long offset, DOMString data);
        // The insertData(offset, data) method must run these steps:
        //
        //     If offset is greater than the context object's
        //     length, throw an INDEX_SIZE_ERR exception and
        //     terminate these steps.
        //
        //     Insert data into the context object's data after
        //     offset UTF-16 code units.
        //
        insertData: constant(function insertData(offset, data) {
            var curtext = this.data;
            if (offset > curtext.length) IndexSizeError();
            var prefix = substring(curtext, 0, offset),
            suffix = substring(curtext, offset);
            this.data = prefix + data + suffix;
        }),


        // void deleteData(unsigned long offset, unsigned long count);
        // The deleteData(offset, count) method must run these steps:
        //
        //     If offset is greater than the context object's
        //     length, throw an INDEX_SIZE_ERR exception and
        //     terminate these steps.
        //
        //     If offset+count is greater than the context
        //     object's length var count be length-offset.
        //
        //     Starting from offset UTF-16 code units remove count
        //     UTF-16 code units from the context object's data.
        deleteData: constant(function deleteData(offset, count) {
            var curtext = this.data, len = curtext.length;

            if (offset > len) IndexSizeError();

            if (offset+count > len)
                count = len - offset;

            var prefix = substring(curtext, 0, offset),
            suffix = substring(curtext, offset+count);

            this.data = prefix + suffix;
        }),


        // void replaceData(unsigned long offset, unsigned long count,
        //                  DOMString data);
        //
        // The replaceData(offset, count, data) method must act as
        // if the deleteData() method is invoked with offset and
        // count as arguments followed by the insertData() method
        // with offset and data as arguments and re-throw any
        // exceptions these methods might have thrown.
        replaceData: constant(function replaceData(offset, count, data) {
            var curtext = this.data, len = curtext.length;

            if (offset > len) IndexSizeError();

            if (offset+count > len)
                count = len - offset;

            var prefix = substring(curtext, 0, offset),
            suffix = substring(curtext, offset+count);

            this.data = prefix + data + suffix;
        }),

        // Utility method that Node.isEqualNode() calls to test Text and
        // Comment nodes for equality.  It is okay to put it here, since
        // Node will have already verified that nodeType is equal
        isEqual: constant(function isEqual(n) {
            return this._data === n._data;
        }),

    });

    return CharacterData;
});


/************************************************************************
 *  src/impl/Text.js
 ************************************************************************/

//@line 1 "src/impl/Text.js"
defineLazyProperty(impl, "Text", function() {
    function Text(doc, data) {
        this.nodeType = TEXT_NODE;
        this.ownerDocument = doc;
        this._data = data;
    }

    var nodeValue = attribute(function() { return this._data; },
                              function(v) {
                                  if (v === this._data) return;
                                  this._data = v;
                                  if (this.rooted)
                                      this.ownerDocument.mutateValue(this);
                                  if (this.parentNode &&
                                      this.parentNode._textchangehook)
                                      this.parentNode._textchangehook(this);
                              });

    Text.prototype = O.create(impl.CharacterData.prototype, {
        _idlName: constant("Text"),
//        nodeType: constant(TEXT_NODE),
        nodeName: constant("#text"),
        // These three attributes are all the same.
        // The data attribute has a [TreatNullAs=EmptyString] but we'll
        // implement that at the interface level
        nodeValue: nodeValue,
        textContent: nodeValue,
        data: nodeValue,
        length: attribute(function() { return this._data.length; }),

        splitText: constant(function splitText(offset) {
            if (offset > this._data.length) IndexSizeError();

            var newdata = substring(this._data, offset),
                newnode = this.ownerDocument.createTextNode(newdata);
            this.data = substring(this.data, 0, offset);

            var parent = this.parentNode;
            if (parent !== null)
                parent.insertBefore(newnode, this.nextSibling);

            return newnode;
        }),

        // XXX
        // wholeText and replaceWholeText() are not implemented yet because
        // the DOMCore specification is considering removing or altering them.
        wholeText: attribute(nyi),
        replaceWholeText: constant(nyi),

        // Utility methods
        clone: constant(function clone() {
            return new impl.Text(this.ownerDocument, this._data);
        }),

    });

    return Text;
});


/************************************************************************
 *  src/impl/Comment.js
 ************************************************************************/

//@line 1 "src/impl/Comment.js"
defineLazyProperty(impl, "Comment", function() {
    function Comment(doc, data) {
        this.nodeType = COMMENT_NODE;
        this.ownerDocument = doc;
        this._data = data;
    }

    var nodeValue = attribute(function() { return this._data; },
                              function(v) {
                                  this._data = v;
                                  if (this.rooted)
                                      this.ownerDocument.mutateValue(this);
                              });

    Comment.prototype = O.create(impl.CharacterData.prototype, {
        _idlName: constant("Comment"),
//        nodeType: constant(COMMENT_NODE),
        nodeName: constant("#comment"),
        nodeValue: nodeValue,
        textContent: nodeValue,
        data: nodeValue,
        length: attribute(function() { return this._data.length; }),

        // Utility methods
        clone: constant(function clone() {
            return new impl.Comment(this.ownerDocument, this._data);
        }),
    });

    return Comment;
});


/************************************************************************
 *  src/impl/ProcessingInstruction.js
 ************************************************************************/

//@line 1 "src/impl/ProcessingInstruction.js"
defineLazyProperty(impl, "ProcessingInstruction", function() {

    function ProcessingInstruction(doc, target, data) {
        this.nodeType = PROCESSING_INSTRUCTION_NODE;
        this.ownerDocument = doc;
        this.target = target;
        this._data = data;
    }

    var nodeValue = attribute(function() { return this._data; },
                              function(v) {
                                  this._data = v;
                                  if (this.rooted)
                                      this.ownerDocument.mutateValue(this);
                              });

    ProcessingInstruction.prototype = O.create(impl.Leaf.prototype, {
        _idlName: constant("ProcessingInstruction"),
//        nodeType: constant(PROCESSING_INSTRUCTION_NODE),
        nodeName: attribute(function() { return this.target; }),
        nodeValue: nodeValue,
        textContent: nodeValue,
        data: nodeValue,

        // Utility methods
        clone: constant(function clone() {
            return new impl.ProcessingInstruction(this.ownerDocument,
                                                  this.target, this._data);
        }),
        isEqual: constant(function isEqual(n) {
            return this.target === n.target && this._data === n._data;
        }),

    });

    return ProcessingInstruction;
});


/************************************************************************
 *  src/impl/Element.js
 ************************************************************************/

//@line 1 "src/impl/Element.js"
defineLazyProperty(impl, "Element", function() {
    function Element(doc, localName, namespaceURI, prefix) {
        this.nodeType = ELEMENT_NODE;
        this.ownerDocument = doc;
        this.localName = localName;
        this.namespaceURI = namespaceURI;
        this.prefix = prefix;

        this.tagName = (prefix !== null)
            ? prefix + ":" + localName
            : localName;

        if (this.isHTML)
            this.tagName = toUpperCase(this.tagName);

        this.childNodes = [];
        this.childNodes._idlName = "NodeList";

        // These properties maintain the set of attributes
        this._attrsByQName = Object.create(null);  // The qname->Attr map
        this._attrsByLName = Object.create(null);  // The ns|lname->Attr map
        this._attrKeys = [];                       // attr index -> ns|lname
    }

    function recursiveGetText(node, a) {
        if (node.nodeType === TEXT_NODE) {
            a.push(node._data);
        }
        else {
            for(var i = 0, n = node.childNodes.length;  i < n; i++)
                recursiveGetText(node.childNodes[i], a);
        }
    }

    function textContentGetter() {
        var strings = [];
        recursiveGetText(this, strings);
        return strings.join("");
    }

    function textContentSetter(newtext) {
        this.removeChildren();
        if (newtext !== null && newtext !== "") {
            this.appendChild(this.ownerDocument.createTextNode(newtext));
        }
    }

    Element.prototype = O.create(impl.Node.prototype, {
        _idlName: constant("Element"),
//        nodeType: constant(ELEMENT_NODE),
        nodeName: attribute(function() { return this.tagName; }),
        nodeValue: attribute(fnull, fnoop),
        textContent: attribute(textContentGetter, textContentSetter),

        children: attribute(function() {
            if (!this._children) {
                this._children = new ChildrenCollection(this);
            }
            return this._children;
        }),

        attributes: attribute(function() {
            if (!this._attributes) {
                this._attributes = new AttributesArray(this);
            }
            return this._attributes;
        }),


        firstElementChild: attribute(function() {
            var kids = this.childNodes;
            for(var i = 0, n = kids.length; i < n; i++) {
                if (kids[i].nodeType === ELEMENT_NODE) return kids[i];
            }
            return null;
        }),

        lastElementChild: attribute(function() {
            var kids = this.childNodes;
            for(var i = kids.length-1; i >= 0; i--) {
                if (kids[i].nodeType === ELEMENT_NODE) return kids[i];
            }
            return null;
        }),

        nextElementSibling: attribute(function() {
            if (this.parentNode) {
                var sibs = this.parentNode.childNodes;
                for(var i = this.index+1, n = sibs.length; i < n; i++) {
                    if (sibs[i].nodeType === ELEMENT_NODE) return sibs[i];
                }
            }
            return null;
        }),

        previousElementSibling: attribute(function() {
            if (this.parentNode) {
                var sibs = this.parentNode.childNodes;
                for(var i = this.index-1; i >= 0; i--) {
                    if (sibs[i].nodeType === ELEMENT_NODE) return sibs[i];
                }
            }
            return null;
        }),

        childElementCount: attribute(function() {
            return this.children.length;
        }),


        // Return the next element, in source order, after this one or
        // null if there are no more.  If root element is specified,
        // then don't traverse beyond its subtree.
        //
        // This is not a DOM method, but is convenient for
        // lazy traversals of the tree.
        nextElement: constant(function(root) {
            var next = this.firstElementChild || this.nextElementSibling;
            if (next) return next;

            if (!root) root = this.ownerDocument.documentElement;

            // If we can't go down or across, then we have to go up
            // and across to the parent sibling or another ancestor's
            // sibling.  Be careful, though: if we reach the root
            // element, or if we reach the documentElement, then
            // the traversal ends.
            for(var parent = this.parentElement;
                parent && parent !== root;
                parent = parent.parentElement) {

                next = parent.nextElementSibling;
                if (next) return next;
            }

            return null;
        }),

        // Just copy this method from the Document prototype
        getElementsByTagName:
            constant(impl.Document.prototype.getElementsByTagName),

        getElementsByTagNameNS:
            constant(impl.Document.prototype.getElementsByTagNameNS),

        getElementsByClassName:
            constant(impl.Document.prototype.getElementsByClassName),


        // Utility methods used by the public API methods above

        isHTML: attribute(function() {
            return this.namespaceURI === HTML_NAMESPACE &&
                this.ownerDocument.isHTML;
        }),

        clone: constant(function clone() {
            var e;

            // XXX:
            // Modify this to use the constructor directly or
            // avoid error checking in some other way. In case we try
            // to clone an invalid node that the parser inserted.
            //
            if (this.namespaceURI !== HTML_NAMESPACE || this.prefix)
                e = this.ownerDocument.createElementNS(this.namespaceURI,
                                                       this.tagName);
            else
                e = this.ownerDocument.createElement(this.localName);

            for(var i = 0, n = this._numattrs; i < n; i++) {
                var a = this._attr(i);
                // Use _ version of the function to avoid error checking
                // in case we're cloning an attribute that is invalid but
                // was inserted by the parser.
                e._setAttributeNS(a.namespaceURI, a.name, a.value);
            }

            return e;
        }),

        isEqual: constant(function isEqual(that) {
            if (this.localName !== that.localName ||
                this.namespaceURI !== that.namespaceURI ||
                this.prefix !== that.prefix ||
                this._numattrs !== that._numattrs)
                return false;

            // Compare the sets of attributes, ignoring order
            // and ignoring attribute prefixes.
            for(var i = 0, n = this._numattrs; i < n; i++) {
                var a = this._attr(i);
                if (!that.hasAttributeNS(a.namespaceURI, a.localName))
                    return false;
                if (that.getAttributeNS(a.namespaceURI,a.localName) !== a.value)
                    return false;
            }

            return true;
        }),

        // This is the "locate a namespace prefix" algorithm from the
        // DOMCore specification.  It is used by Node.lookupPrefix()
        locateNamespacePrefix: constant(function locateNamespacePrefix(ns) {
            if (this.namespaceURI === ns && this.prefix !== null)
                return this.prefix;

            for(var i = 0, n = this._numattrs; i < n; i++) {
                var a = this._attr(i);
                if (a.prefix === "xmlns" && a.value === ns)
                    return a.localName;
            }

            var parent = this.parentElement;
            return parent ? parent.locateNamespacePrefix(ns) : null;
        }),

        // This is the "locate a namespace" algorithm for Element nodes
        // from the DOM Core spec.  It is used by Node.lookupNamespaceURI
        locateNamespace: constant(function locateNamespace(prefix) {
            if (this.prefix === prefix && this.namespaceURI !== null)
                return this.namespaceURI;

            for(var i = 0, n = this._numattrs; i < n; i++) {
                var a = this._attr(i);
                if ((a.prefix === "xmlns" && a.localName === prefix) ||
                    (a.prefix === null && a.localName === "xmlns")) {
                    return a.value || null;
                }
            }

            var parent = this.parentElement;
            return parent ? parent.locateNamespace(prefix) : null;
        }),

        //
        // Attribute handling methods and utilities
        //

        /*
         * Attributes in the DOM are tricky:
         *
         * - there are the 8 basic get/set/has/removeAttribute{NS} methods
         *
         * - but many HTML attributes are also "reflected" through IDL
         *   attributes which means that they can be queried and set through
         *   regular properties of the element.  There is just one attribute
         *   value, but two ways to get and set it.
         *
         * - Different HTML element types have different sets of reflected
             attributes.
         *
         * - attributes can also be queried and set through the .attributes
         *   property of an element.  This property behaves like an array of
         *   Attr objects.  The value property of each Attr is writeable, so
         *   this is a third way to read and write attributes.
         *
         * - for efficiency, we really want to store attributes in some kind
         *   of name->attr map.  But the attributes[] array is an array, not a
         *   map, which is kind of unnatural.
         *
         * - When using namespaces and prefixes, and mixing the NS methods
         *   with the non-NS methods, it is apparently actually possible for
         *   an attributes[] array to have more than one attribute with the
         *   same qualified name.  And certain methods must operate on only
         *   the first attribute with such a name.  So for these methods, an
         *   inefficient array-like data structure would be easier to
         *   implement.
         *
         * - The attributes[] array is live, not a snapshot, so changes to the
         *   attributes must be immediately visible through existing arrays.
         *
         * - When attributes are queried and set through IDL properties
         *   (instead of the get/setAttributes() method or the attributes[]
         *   array) they may be subject to type conversions, URL
         *   normalization, etc., so some extra processing is required in that
         *   case.
         *
         * - But access through IDL properties is probably the most common
         *   case, so we'd like that to be as fast as possible.
         *
         * - We can't just store attribute values in their parsed idl form,
         *   because setAttribute() has to return whatever string is passed to
         *   getAttribute even if it is not a legal, parseable value. So
         *   attribute values must be stored in unparsed string form.
         *
         * - We need to be able to send change notifications or mutation
         *   events of some sort to the renderer whenever an attribute value
         *   changes, regardless of the way in which it changes.
         *
         * - Some attributes, such as id and class affect other parts of the
         *   DOM API, like getElementById and getElementsByClassName and so
         *   for efficiency, we need to specially track changes to these
         *   special attributes.
         *
         * - Some attributes like class have different names (className) when
         *   reflected.
         *
         * - Attributes whose names begin with the string "data-" are treated
             specially.
         *
         * - Reflected attributes that have a boolean type in IDL have special
         *   behavior: setting them to false (in IDL) is the same as removing
         *   them with removeAttribute()
         *
         * - numeric attributes (like HTMLElement.tabIndex) can have default
         *   values that must be returned by the idl getter even if the
         *   content attribute does not exist. (The default tabIndex value
         *   actually varies based on the type of the element, so that is a
         *   tricky one).
         *
         * See
         * http://www.whatwg.org/specs/web-apps/current-work/multipage/urls.html#reflect
         * for rules on how attributes are reflected.
         *
         */

        getAttribute: constant(function getAttribute(qname) {
            if (this.isHTML) qname = toLowerCase(qname);
            var attr = this._attrsByQName[qname];
            if (!attr) return null;

            if (isArray(attr))  // If there is more than one
                attr = attr[0];   // use the first

            return attr.value;
        }),

        getAttributeNS: constant(function getAttributeNS(ns, lname) {
            var attr = this._attrsByLName[ns + "|" + lname];
            return attr ? attr.value : null;
        }),

        hasAttribute: constant(function hasAttribute(qname) {
            if (this.isHTML) qname = toLowerCase(qname);
            return qname in this._attrsByQName;
        }),

        hasAttributeNS: constant(function hasAttributeNS(ns, lname) {
            var key = ns + "|" + lname;
            return key in this._attrsByLName;
        }),

        // Set the attribute without error checking. The parser uses this.
        _setAttribute: constant(function _setAttribute(qname, value) {
            // XXX: the spec says that this next search should be done
            // on the local name, but I think that is an error.
            // email pending on www-dom about it.
            var attr = this._attrsByQName[qname];
            var isnew;
            if (!attr) {
                attr = this._newattr(qname);
                isnew = true;
            }
            else {
                if (isArray(attr)) attr = attr[0];
            }

            // Now set the attribute value on the new or existing Attr object.
            // The Attr.value setter method handles mutation events, etc.
            attr.value = value;

            if (isnew && this._newattrhook) this._newattrhook(qname, value);
        }),

        // Check for errors, and then set the attribute
        setAttribute: constant(function setAttribute(qname, value) {
            if (!xml.isValidName(qname)) InvalidCharacterError();
            if (this.isHTML) qname = toLowerCase(qname);
            if (substring(qname, 0, 5) === "xmlns") NamespaceError();
            this._setAttribute(qname, value);
        }),


        // The version with no error checking used by the parser
        _setAttributeNS: constant(function _setAttributeNS(ns, qname, value) {
            var pos = S.indexOf(qname, ":"), prefix, lname;
            if (pos === -1) {
                prefix = null;
                lname = qname;
            }
            else {
                prefix = substring(qname, 0, pos);
                lname = substring(qname, pos+1);
            }

            var key = ns + "|" + lname;
            if (ns === "") ns = null;

            var attr = this._attrsByLName[key];
            var isnew;
            if (!attr) {
                var attr = new Attr(this, lname, prefix, ns);
                isnew = true;
                this._attrsByLName[key] = attr;
                this._attrKeys = O.keys(this._attrsByLName);

                // We also have to make the attr searchable by qname.
                // But we have to be careful because there may already
                // be an attr with this qname.
                this._addQName(attr);
            }
            else {
                // Calling setAttributeNS() can change the prefix of an
                // existing attribute!
                if (attr.prefix !== prefix) {
                    // Unbind the old qname
                    this._removeQName(attr);
                    // Update the prefix
                    attr.prefix = prefix;
                    // Bind the new qname
                    this._addQName(attr);

                }

            }
            attr.value = value; // Automatically sends mutation event
            if (isnew && this._newattrhook) this._newattrhook(qname, value);
        }),

        // Do error checking then call _setAttributeNS
        setAttributeNS: constant(function setAttributeNS(ns, qname, value) {
            if (!xml.isValidName(qname)) InvalidCharacterError();
            if (!xml.isValidQName(qname)) NamespaceError();

            var pos = S.indexOf(qname, ":");
            var prefix = (pos === -1) ? null : substring(qname, 0, pos);
            if (ns === "") ns = null;

            if ((prefix !== null && ns === null) ||
                (prefix === "xml" && ns !== XML_NAMESPACE) ||
                ((qname === "xmlns" || prefix === "xmlns") &&
                 (ns !== XMLNS_NAMESPACE)) ||
                (ns === XMLNS_NAMESPACE &&
                 !(qname === "xmlns" || prefix === "xmlns")))
                NamespaceError();

            this._setAttributeNS(ns, qname, value);
        }),

        removeAttribute: constant(function removeAttribute(qname) {
            if (this.isHTML) qname = toLowerCase(qname);

            var attr = this._attrsByQName[qname];
            if (!attr) return;

            // If there is more than one match for this qname
            // so don't delete the qname mapping, just remove the first
            // element from it.
            if (isArray(attr)) {
                if (attr.length > 2) {
                    attr = A.shift(attr);  // remove it from the array
                }
                else {
                    this._attrsByQName[qname] = attr[1];
                    attr = attr[0];
                }
            }
            else {
                // only a single match, so remove the qname mapping
                delete this._attrsByQName[qname];
            }

            // Now attr is the removed attribute.  Figure out its
            // ns+lname key and remove it from the other mapping as well.
            var key = (attr.namespaceURI || "") + "|" + attr.localName;
            delete this._attrsByLName[key];
            this._attrKeys = O.keys(this._attrsByLName);

            // Onchange handler for the attribute
            if (attr.onchange)
                attr.onchange(this, attr.localName, attr.value, null);

            // Mutation event
            if (this.rooted) this.ownerDocument.mutateRemoveAttr(attr);
        }),

        removeAttributeNS: constant(function removeAttributeNS(ns, lname) {
            var key = (ns || "") + "|" + lname;
            var attr = this._attrsByLName[key];
            if (!attr) return;

            delete this._attrsByLName[key];
            this._attrKeys = O.keys(this._attrsByLName);

            // Now find the same Attr object in the qname mapping and remove it
            // But be careful because there may be more than one match.
            this._removeQName(attr);

            // Onchange handler for the attribute
            if (attr.onchange)
                attr.onchange(this, attr.localName, attr.value, null);
            // Mutation event
            if (this.rooted) this.ownerDocument.mutateRemoveAttr(attr);
        }),

        // This "raw" version of getAttribute is used by the getter functions
        // of reflected attributes. It skips some error checking and
        // namespace steps
        _getattr: constant(function _getattr(qname) {
            // Assume that qname is already lowercased, so don't do it here.
            // Also don't check whether attr is an array: a qname with no
            // prefix will never have two matching Attr objects (because
            // setAttributeNS doesn't allow a non-null namespace with a
            // null prefix.
            var attr = this._attrsByQName[qname];
            return attr ? attr.value : null;
        }),

        // The raw version of setAttribute for reflected idl attributes.
        _setattr: constant(function _setattr(qname, value) {
            var attr = this._attrsByQName[qname];
            var isnew;
            if (!attr) {
                attr = this._newattr(qname);
                isnew = true;
            }
            attr.value = value;
            if (isnew && this._newattrhook) this._newattrhook(qname, value);
        }),

        // Create a new Attr object, insert it, and return it.
        // Used by setAttribute() and by set()
        _newattr: constant(function _newattr(qname) {
            var attr = new Attr(this, qname);
            this._attrsByQName[qname] = attr;
            this._attrsByLName["|" + qname] = attr;
            this._attrKeys = O.keys(this._attrsByLName);
            return attr;
        }),

        // Add a qname->Attr mapping to the _attrsByQName object, taking into
        // account that there may be more than one attr object with the
        // same qname
        _addQName: constant(function(attr) {
            var qname = attr.name;
            var existing = this._attrsByQName[qname];
            if (!existing) {
                this._attrsByQName[qname] = attr;
            }
            else if (isArray(existing)) {
                push(existing, attr);
            }
            else {
                this._attrsByQName[qname] = [existing, attr];
            }
        }),

        // Remove a qname->Attr mapping to the _attrsByQName object, taking into
        // account that there may be more than one attr object with the
        // same qname
        _removeQName: constant(function(attr) {
            var qname = attr.name;
            var target = this._attrsByQName[qname];

            if (isArray(target)) {
                var idx = A.indexOf(target, attr);
                assert(idx !== -1); // It must be here somewhere
                if (target.length === 2) {
                    this._attrsByQName[qname] = target[1-idx];
                }
                else {
                    splice(target, idx, 1)
                }
            }
            else {
                assert(target === attr);  // If only one, it must match
                delete this._attrsByQName[qname];
            }
        }),

        // Return the number of attributes
        _numattrs: attribute(function() { return this._attrKeys.length; }),
        // Return the nth Attr object
        _attr: constant(function(n) {
            return this._attrsByLName[this._attrKeys[n]];
        }),
    });

    // A utility function used by those below
    function defineAttribute(c, idlname, getter, setter) {
        // I don't think we should ever override an existing attribute
        assert(!(idlname in c.prototype), "Redeclared attribute " + idlname);
        O.defineProperty(c.prototype, idlname, { get: getter, set: setter });
    }


    // This is a utility function for setting up reflected attributes.
    // Pass an element impl class like impl.HTMLElement as the first
    // argument.  Pass the content attribute name as the second argument.
    // And pass the idl attribute name as the third, if it is different.
    Element.reflectStringAttribute = function(c, name, idlname) {
        defineAttribute(c, idlname || name,
                        function() { return this._getattr(name) || ""; },
                        function(v) { this._setattr(name, v); });
    };

    // Define an idl attribute that reflects an enumerated content
    // attribute.  This is for attributes that the HTML spec describes as
    // "limited to only known values".  legalvals should be an array that
    // maps the lowercased versions of allowed enumerated values to the
    // canonical value that it should convert to. Usually the name and
    // value of most properties in the object will be the same.
    Element.reflectEnumeratedAttribute = function(c, name, idlname, legalvals,
                                                  missing_default,
                                                  invalid_default)
    {
        defineAttribute(c, idlname || name,
                        function() {
                            var v = this._getattr(name);
                            if (v === null) return missing_default || "";

                            v = legalvals[v.toLowerCase()];
                            if (v !== undefined)
                                return v;
                            if (invalid_default !== undefined)
                                return invalid_default;
                            if (missing_default !== undefined)
                                return missing_default;
                            return "";
                        },
                        function(v) { this._setattr(name, v); });
    };

    Element.reflectBooleanAttribute = function(c, name, idlname) {
        defineAttribute(c, idlname || name,
                        function() {
                            return this.hasAttribute(name);
                        },
                        function(v) {
                            if (v) {
                                this._setattr(name, "");
                            }
                            else {
                                this.removeAttribute(name);
                            }
                        });
    };

    // See http://www.whatwg.org/specs/web-apps/current-work/#reflect
    //
    // defval is the default value. If it is a function, then that function
    // will be invoked as a method of the element to obtain the default.
    // If no default is specified for a given attribute, then the default
    // depends on the type of the attribute, but since this function handles
    // 4 integer cases, you must specify the default value in each call
    //
    // min and max define a valid range for getting the attribute.
    //
    // setmin defines a minimum value when setting.  If the value is less
    // than that, then throw INDEX_SIZE_ERR.
    //
    // Conveniently, JavaScript's parseInt function appears to be
    // compatible with HTML's "rules for parsing integers"
    Element.reflectIntegerAttribute = function(c, name, defval, idlname,
                                               min, max, setmin)
    {
        var getter, setter;

        if (min != null ||
            max != null ||
            typeof defval === "function") {
            getter = function() {
                var v = this._getattr(name);
                var n = parseInt(v, 10);
                if (isNaN(n) ||
                    (min != null && n < min) ||
                    (max != null && n > max)) {
                    switch(typeof defval) {
                    case 'function': return defval.call(this);
                    case 'number': return defval;
                    default: assert(false);
                    }
                }

                return n;
            };
        }
        else {
            getter = function() {
                var v = this._getattr(name);
                // Pleasantly, JavaScript's parseInt function
                // is compatible with HTML's "rules for parsing
                // integers"
                var n = parseInt(v, 10);
                return isNaN(n) ? defval : n;
            }
        }

        if (setmin != null) {
            setter = function(v) {
                if (v < setmin) IndexSizeError(name + " set to " + v);
                this._setattr(name, String(v));
            };
        }
        else {
            setter = function(v) {
                this._setattr(name, String(v));
            };
        }

        defineAttribute(c, idlname || name, getter, setter);
    };

    Element.reflectFloatAttribute = function(c, name, defval, idlname) {
        defineAttribute(c, idlname || name,
                        function() {
                            var s = this._getattr(name), x;
                            if (s) x = parseFloat();
                            return (x && isFinite(x)) ? x : defval;
                        },
                        function(v) {
                            this._setattr(name, String(v));
                        });
    };

    Element.reflectPositiveFloatAttribute = function(c, name, defval, idlname) {
        defineAttribute(c, idlname || name,
                        function() {
                            var s = this._getattr(name), x;
                            if (s) x = parseFloat(s);
                            return (x && isFinite(x) && x > 0) ? x : defval;
                        },
                        function(v) {
                            if (v < 0) return; // Ignore negative values
                            this._setattr(name, String(v));
                        });
    };


    // This is a utility function for setting up change handler functions
    // for attributes like 'id' that require special handling when they change.
    Element.registerAttributeChangeHandler = function(c, name, handler) {
        var p = c.prototype;

        // If p does not already have its own _attributeChangeHandlers
        // then create one for it, inheriting from the inherited
        // _attributeChangeHandlers. At the top (for the impl.Element
        // class) the _attributeChangeHandlers object will be created
        // with a null prototype.
        if (!hasOwnProperty(p, "_attributeChangeHandlers")) {
            p._attributeChangeHandlers =
                Object.create(p._attributeChangeHandlers || null);
        }

        // There can only be one
        // XXX: I've commented out this assertion.  Actually, HTMLBodyElement
        // wants to override the attribute change handlers for certain
        // event handler attributes it inherits from HTMLElement...
        // assert(!(name in p._attributeChangeHandlers));

        p._attributeChangeHandlers[name] = handler;
    };



    // Register special handling for the id attribute
    Element.registerAttributeChangeHandler(Element, "id",
               function(element, lname, oldval, newval) {
                   if (element.rooted) {
                       if (oldval) {
                           element.ownerDocument.delId(oldval, element);
                       }
                       if (newval) {
                           element.ownerDocument.addId(newval, element);
                       }
                   }
               });

    // Define getters and setters for an "id" property that reflects
    // the content attribute "id".
    Element.reflectStringAttribute(Element, "id");

    // Define getters and setters for a "className" property that reflects
    // the content attribute "class".
    Element.reflectStringAttribute(Element, "class", "className");


    // The Attr class represents a single attribute.  The values in
    // _attrsByQName and _attrsByLName are instances of this class.
    function Attr(elt, lname, prefix, namespace) {
        // Always remember what element we're associated with.
        // We need this to property handle mutations
        this.ownerElement = elt;

        if (!namespace && !prefix && lname in elt._attributeChangeHandlers)
            this.onchange = elt._attributeChangeHandlers[lname];

        // localName and namespace are constant for any attr object.
        // But value may change.  And so can prefix, and so, therefore can name.
        this.localName = lname;
        this.prefix = prefix || null;
        this.namespaceURI = namespace || null;
    }

    Attr.prototype = {
        _idlName: "Attr",
        get name() {
            return this.prefix
                ? this.prefix + ":" + this.localName
                : this.localName;
        },

        get value() {
            return this.data;
        },

        set value(v) {
            if (this.data === v) return;
            var oldval = this.data;
            this.data = v;

            // Run the onchange hook for the attribute
            // if there is one.
            if (this.onchange)
                this.onchange(this.ownerElement,this.localName, oldval, v);

            // Generate a mutation event if the element is rooted
            if (this.ownerElement.rooted)
                this.ownerElement.ownerDocument.mutateAttr(
                    this,
                    oldval);
        }
    };


    // The attributes property of an Element will be an instance of this class.
    // This class is really just a dummy, though. It only defines a length
    // property and an item() method. The AttrArrayProxy that
    // defines the public API just uses the Element object itself.  But in
    // order to get wrapped properly, we need to return an object with the
    // right _idlName property
    function AttributesArray(elt) { this.element = elt; }
    AttributesArray.prototype = {
        _idlName: "AttrArray",
        get length() {
            return this.element._attrKeys.length;
        },
        item: function(n) {
            return this.element._attrsByLName[this.element._attrKeys[n]];
        }
    };


    // The children property of an Element will be an instance of this class.
    // It defines length, item() and namedItem() and will be wrapped by an
    // HTMLCollection when exposed through the DOM.
    function ChildrenCollection(e) {
        this.element = e;
    }

    ChildrenCollection.prototype = {
        _idlName: "HTMLCollection",
        get length() {
            this.updateCache();
            return this.childrenByNumber.length;
        },

        item: function item(n) {
            this.updateCache();
            return this.childrenByNumber[n] || null;
        },

        namedItem: function namedItem(name) {
            this.updateCache();
            return this.childrenByName[name] || null;
        },

        // This attribute returns the entire name->element map.
        // It is not part of the HTMLCollection API, but we need it in
        // src/HTMLCollectionProxy
        get namedItems() {
            this.updateCache();
            return this.childrenByName;
        },

        updateCache: function updateCache() {
            var namedElts = /^(a|applet|area|embed|form|frame|frameset|iframe|img|object)$/;
            if (this.lastModTime !== this.element.lastModTime) {
                this.lastModTime = this.element.lastModTime;
                this.childrenByNumber = [];
                this.childrenByName = {};

                for(var i = 0, n = this.element.childNodes.length; i < n; i++) {
                    var c = this.element.childNodes[i];
                    if (c.nodeType == ELEMENT_NODE) {
                        push(this.childrenByNumber, c);

                        // XXX Are there any requirements about the namespace
                        // of the id property?
                        var id = c.getAttribute("id");

                        // If there is an id that is not already in use...
                        if (id && !this.childrenByName[id])
                            this.childrenByName[id] = c;

                        // For certain HTML elements we check the name attribute
                        var name = c.getAttribute("name");
                        if (name &&
                            this.element.namespaceURI === HTML_NAMESPACE &&
                            namedElts.test(this.element.localName) &&
                            !this.childrenByName[name])
                            this.childrenByName[id] = c;
                    }
                }
            }
        }
    };

    return Element;
});



/************************************************************************
 *  src/impl/MutationConstants.js
 ************************************************************************/

//@line 1 "src/impl/MutationConstants.js"
// The value of a Text, Comment or PI node changed
const MUTATE_VALUE = 1;

// A new attribute was added or an attribute value and/or prefix changed
const MUTATE_ATTR = 2;

// An attribute was removed
const MUTATE_REMOVE_ATTR = 3;

// A node was removed
const MUTATE_REMOVE = 4;

// A node was moved
const MUTATE_MOVE = 5;

// A node (or a subtree of nodes) was inserted
const MUTATE_INSERT = 6;


/************************************************************************
 *  src/impl/domstr.js
 ************************************************************************/

//@line 1 "src/impl/domstr.js"
// A string representation of DOM trees
var DOMSTR = (function() {
    const NUL = "\0";

    const HTML_NAMESPACE = "http://www.w3.org/1999/xhtml";
    const XML_NAMESPACE = "http://www.w3.org/XML/1998/namespace";
    const XMLNS_NAMESPACE = "http://www.w3.org/2000/xmlns/";
    const MATHML_NAMESPACE = "http://www.w3.org/1998/Math/MathML";
    const SVG_NAMESPACE = "http://www.w3.org/2000/svg";

    const substring = String.substring;
    const indexOf = String.indexOf;
    const charCodeAt = String.charCodeAt;
    const fromCharCode = String.fromCharCode;

    function serialize(n) {
        function serializeNode(n) {
            switch (n.nodeType) {
            case Node.TEXT_NODE:
                return n.data;
            case Node.COMMENT_NODE:
                return {comment: n.data};
            case Node.PROCESSING_INSTRUCTION_NODE:
                return {pi: n.target, data: n.data};
            case Node.DOCUMENT_TYPE_NODE:
                // HTML ignores the publicID and systemID when
                // serializing nodes, so ignore them here, too
                return {doctype: n.name};
            case Node.ELEMENT_NODE:
                return serializeElement(n);
            case NODE.DOCUMENT_FRAGMENT_NODE:
                return serializeFragment(n);
            }
        }

        function serializeElement(n) {
            var elt = {};
            if (n.namespaceURI === HTML_NAMESPACE && !n.prefix) {
                elt.html = n.localName;
            }
            else {
                elt.ns = serializeNamespace(n.namespaceURI);
                elt.tag = n.tagName;
            }

            // Handle the attributes
            if (n.attributes.length) {
                elt.attr = new Array(n.attributes.length);
            }
            for(var i = 0, l = n.attributes.length; i < l; i++) {
                elt.attr[i] = serializeAttr(n.attributes.item(i));
            }

            // Now the children
            if (n.childNodes.length) {
                elt.child = new Array(n.childNodes.length);
            }
            for(var i = 0, l = n.childNodes.length; i < l; i++) {
                elt.child[i] = serializeNode(n.childNodes[i]);
            }

            return elt;
        }

        var lastCustomNS = null;

        function serializeNamespace(ns) {
            switch(ns) {
            case HTML_NAMESPACE: return "h";
            case null: return "u";
            case XML_NAMESPACE: return "x";
            case XMLNS_NAMESPACE: return "n";
            case MATHML_NAMESPACE: return "m";
            case SVG_NAMESPACE: return "s";
            default:
                if (ns === lastCustomNS) return "l"
                else {
                    lastCustomNS = ns;
                    return "c" + ns;
                }
            }
        }

        function serializeAttr(a) {
            if (a.namespaceURI === null && a.prefix === null) {
                // set with setAttribute()
                return {a: a.name, v: a.value};
            }
            else {
                // set with setAttributeNS()
                return {ns: serializeNamespace(a.namespaceURI),
                    a: a.name, v: a.value};
            }
        }

        function serializeLength(n) {
            if (n < 0) throw new Error("negative length");
            if (n <= 0xD7FF) return fromCharCode(n);
            else return fromCharCode("0xFFFF") + String(n) + NUL;
        }

        function serializeFragment(n) {
            var frag = {frag: new Array(n.childNodes.length)};
            for(var i = 0, l = n.childNodes.length; i < l; i++) {
                frag[i] = serializeNode(n.childNodes[i]);
            }
            return frag;
        }

        return serializeNode(n);
    }


    function parse(node, d) {
        if (!d) d = document;

        function parseNode(n) {
            if (typeof n === "string") {
                return d.createTextNode(n);
            }
            if (n.comment !== undefined) {
                return d.createComment(n.comment);
            }
            if (n.pi !== undefined) {
                return d.createProcessingInstruction(n.pi, n.data);
            }
            if (n.doctype !== undefined) {
                return d.implementation.createDocumentType(n.doctype, "", "");
            }
            if (n.html !== undefined) {
                return parseElement("H", n);
            }
            if (n.ns !== undefined) {
                return parseElement("E", n);
            }
            if (n.frag !== undefined) {
                return parseFragment(n);
            }
        }

        function parseElement(type, n) {
            var e;
            if (type === "H")
                e = d.createElement(n.html);
            else
                e = d.createElementNS(parseNamespace(n.ns), n.tag);

            var numattrs = 0;
            if (n.attr !== undefined) {
                numattrs = n.attr.length;
            }
            for(var i = 0; i < numattrs; i++) {
                var attr = n.attr[i];
                if (attr.a !== undefined)
                    e.setAttribute(attr.a, attr.v);
                else
                    e.setAttributeNS(attr.ns, attr.a, attr.v);
            }

            var numkids = 0;
            if (n.child !== undefined) {
                numkids = n.child.length;
            }
            for(var i = 0; i < numkids; i++) {
                e.appendChild(parseNode(n.child[i]));
            }

            return e;
        }


        var lastCustomNS = null;

        function parseNamespace(n) {
            switch(n[0]) {
            case 'h': return HTML_NAMESPACE;
            case 'u': return null;
            case 'x': return XML_NAMESPACE;
            case 'n': return XMLNS_NAMESPACE;
            case 'm': return MATHML_NAMESPACE;
            case 's': return SVG_NAMESPACE;
            case 'l': return lastCustomNS;
            case 'c':
                return n.slice(1);
            }
        }

        function parseFragment(n) {
            var f = d.createDocumentFragment();
            var len = n.frag.length;
            for(var i = 0; i < len; i++)
                f.appendChild(parseNode(n.frag[i]));
            return f;
        }

        return parseNode(node);
    }

    return { serialize: serialize, parse: parse };
}());



/************************************************************************
 *  src/impl/Document.js
 ************************************************************************/

//@line 1 "src/impl/Document.js"
defineLazyProperty(impl, "Document", function() {

    function Document(isHTML, address) {
        this.nodeType = DOCUMENT_NODE;
        this.isHTML = isHTML;
        this._address = address || "about:blank";
        this.implementation = new impl.DOMImplementation();

        // DOMCore says that documents are always associated with themselves.
        this.ownerDocument = this;

        // These will be initialized by our custom versions of
        // appendChild and insertBefore that override the inherited
        // Node methods.
        // XXX: override those methods!
        this.doctype = null;
        this.documentElement = null;
        this.childNodes = [];
        this.childNodes._idlName = "NodeList";

        // Documents are always rooted, by definition
        this._nid = 1;
        this._nextnid = 2; // For numbering children of the document
        this._nodes = [null, this];  // nid to node map

        // This maintains the mapping from element ids to element nodes.
        // We may need to update this mapping every time a node is rooted
        // or uprooted, and any time an attribute is added, removed or changed
        // on a rooted element.
        this.byId = O.create(null); // inherit nothing

        // This property holds a monotonically increasing value akin to
        // a timestamp used to record the last modification time of nodes
        // and their subtrees. See the lastModTime attribute and modify()
        // method of the Node class.  And see FilteredElementList for an example
        // of the use of lastModTime
        this.modclock = 0;
    }

    // Map from lowercase event category names (used as arguments to
    // createEvent()) to the property name in the impl object of the
    // event constructor.
    var supportedEvents = {
        event: "Event",
        customevent: "CustomEvent",
        uievent: "UIEvent",
        mouseevent: "MouseEvent",
        keyboardevent: "KeyboardEvent",
        wheelevent: "WheelEvent",
        voiceevent: "VoiceEvent",
        spatialinputevent: "SpatialInputEvent",
    };

    // Certain arguments to document.createEvent() must be treated specially
    var replacementEvent = {
        htmlevents: "event",
        mouseevents: "mouseevent",
        mutationevents: "mutationevent",
        uievents: "uievent"
    };

    Document.prototype = O.create(impl.Node.prototype, {
        _idlName: constant("Document"),


        // This method allows dom.js to communicate with a renderer
        // that displays the document in some way
        // XXX: I should probably move this to the window object
        _setMutationHandler: constant(function(handler) {
            this.mutationHandler = handler;
        }),

        // This method allows dom.js to receive event notifications
        // from the renderer.
        // XXX: I should probably move this to the window object
        _dispatchRendererEvent: constant(function(targetNid, type, details) {
            var target = this._nodes[targetNid];
            if (!target) return;
            target._dispatchEvent(new impl.Event(type, details), true);
        }),

//        nodeType: constant(DOCUMENT_NODE),
        nodeName: constant("#document"),
        nodeValue: attribute(fnull, fnoop),

        // XXX: DOMCore may remove documentURI, so it is NYI for now
        documentURI: attribute(nyi, nyi),
        compatMode: attribute(function() {
            // The _quirks property is set by the HTML parser
            return this._quirks ? "BackCompat" : "CSS1Compat";
        }),
        parentNode: constant(null),

        createTextNode: constant(function(data) {
            return new impl.Text(this, data);
        }),
        createComment: constant(function(data) {
            return new impl.Comment(this, data);
        }),
        createDocumentFragment: constant(function() {
            return new impl.DocumentFragment(this);
        }),
        createProcessingInstruction: constant(function(target, data) {
            if (this.isHTML) NotSupportedError();
            if (!xml.isValidName(target) || S.indexOf(data, "?>") !== -1)
                InvalidCharacterError();
            return new impl.ProcessingInstruction(this, target, data);
        }),

        createElement: constant(function(localName) {
            if (!xml.isValidName(localName)) InvalidCharacterError();

            if (this.isHTML)
                localName = toLowerCase(localName);

            var interfaceName = tagNameToInterfaceName[localName] ||
                "HTMLUnknownElement";
            return new impl[interfaceName](this, localName, null);
        }),

        createElementNS: constant(function(namespace, qualifiedName) {
            if (!xml.isValidName(qualifiedName)) InvalidCharacterError();
            if (!xml.isValidQName(qualifiedName)) NamespaceError();

            var pos, prefix, localName;
            if ((pos = S.indexOf(qualifiedName, ":")) !== -1) {
                prefix = substring(qualifiedName, 0, pos);
                localName = substring(qualifiedName, pos+1);

                if (namespace === "" ||
                    (prefix === "xml" && namespace !== XML_NAMESPACE))
                    NamespaceError();
            }
            else {
                prefix = null;
                localName = qualifiedName;
            }

            if (((qualifiedName === "xmlns" || prefix === "xmlns") &&
                 namespace !== XMLNS_NAMESPACE) ||
                (namespace === XMLNS_NAMESPACE &&
                 qualifiedName !== "xmlns" &&
                 prefix !== "xmlns"))
                NamespaceError();

            if (namespace === HTML_NAMESPACE) {
                var interfaceName = tagNameToInterfaceName[localName] ||
                    "HTMLUnknownElement";
                return new impl[interfaceName](this, localName, prefix);
            }

            return new impl.Element(this, localName, namespace, prefix);
        }),

        createEvent: constant(function createEvent(interfaceName) {
            interfaceName = toLowerCase(interfaceName);
            var name = replacementEvent[interfaceName] || interfaceName;
            var constructor = impl[supportedEvents[name]];

            if (constructor) {
                var e = new constructor();
                e._initialized = false;
                return e;
            }
            else {
                NotSupportedError();
            }
        }),

        dispatchMouseFromWindow: constant(function dispatchMouseFromWindow(event) {
            var success = this.dispatchEvent(event);
        }),


        // Add some (surprisingly complex) document hierarchy validity
        // checks when adding, removing and replacing nodes into a
        // document object, and also maintain the documentElement and
        // doctype properties of the document.  Each of the following
        // 4 methods chains to the Node implementation of the method
        // to do the actual inserting, removal or replacement.

        appendChild: constant(function(child) {
            if (child.nodeType === TEXT_NODE) HierarchyRequestError();
            if (child.nodeType === ELEMENT_NODE) {
                if (this.documentElement) // We already have a root element
                    HierarchyRequestError();

                this.documentElement = child;
            }
            if (child.nodeType === DOCUMENT_TYPE_NODE) {
                if (this.doctype ||        // Already have one
                    this.documentElement)   // Or out-of-order
                    HierarchyRequestError()

                this.doctype = child;
            }

            // Now chain to our superclass
            return call(impl.Node.prototype.appendChild, this, child);
        }),

        insertBefore: constant(function insertBefore(child, refChild) {
            if (refChild === null) return call(impl.Document.prototype.appendChild, this, child);
            if (refChild.parentNode !== this) NotFoundError();
            if (child.nodeType === TEXT_NODE) HierarchyRequestError();
            if (child.nodeType === ELEMENT_NODE) {
                // If we already have a root element or if we're trying to
                // insert it before the doctype
                if (this.documentElement ||
                    (this.doctype && this.doctype.index >= refChild.index))
                    HierarchyRequestError();

                this.documentElement = child;
            }
            if (child.nodeType === DOCUMENT_TYPE_NODE) {
                if (this.doctype ||
                    (this.documentElement &&
                     refChild.index > this.documentElement.index))
                    HierarchyRequestError()

                this.doctype = child;
            }
            return call(impl.Node.prototype.insertBefore,this, child, refChild);
        }),

        replaceChild: constant(function replaceChild(child, oldChild) {
            if (oldChild.parentNode !== this) NotFoundError();

            if (child.nodeType === TEXT_NODE) HierarchyRequestError();
            if (child.nodeType === ELEMENT_NODE) {
                // If we already have a root element and we're not replacing it
                if (this.documentElement && this.documentElement !== oldChild)
                    HierarchyRequestError();
                // Or if we're trying to put the element before the doctype
                // (replacing the doctype is okay)
                if (this.doctype && oldChild.index < this.doctype.index)
                    HierarchyRequestError();

                if (oldChild === this.doctype) this.doctype = null;
            }
            else if (child.nodeType === DOCUMENT_TYPE_NODE) {
                // If we already have a doctype and we're not replacing it
                if (this.doctype && oldChild !== this.doctype)
                    HierarchyRequestError();
                // If we have a document element and the old child
                // comes after it
                if (this.documentElement &&
                    oldChild.index > this.documentElement.index)
                    HierarchyRequestError();

                if (oldChild === this.documentElement)
                    this.documentElement = null;
            }
            else {
                if (oldChild === this.documentElement)
                    this.documentElement = null;
                else if (oldChild === this.doctype)
                    this.doctype = null;
            }
            return call(impl.Node.prototype.replaceChild, this,child,oldChild);
        }),

        removeChild: constant(function removeChild(child) {
            if (child.nodeType === DOCUMENT_TYPE_NODE)
                this.doctype = null;
            else if (child.nodeType === ELEMENT_NODE)
                this.documentElement = null;

            // Now chain to our superclass
            return call(impl.Node.prototype.removeChild, this, child);
        }),

        getElementById: constant(function(id) {
            var n = this.byId[id];
            if (!n) return null;
            if (isArray(n)) { // there was more than one element with this id
                return n[0];  // array is sorted in document order
            }
            return n;
        }),


        // XXX:
        // Tests are currently failing for this function.
        // Awaiting resolution of:
        // http://lists.w3.org/Archives/Public/www-dom/2011JulSep/0016.html
        getElementsByTagName: constant(function getElementsByTagName(lname) {
            var filter;
            if (lname === "*")
                filter = ftrue;
            else if (this.doc.isHTML)
                filter = htmlLocalNameElementFilter(lname);
            else
                filter = localNameElementFilter(lname);

            return new impl.FilteredElementList(this, filter);
        }),

        getElementsByTagNameNS: constant(function getElementsByTagNameNS(ns,
                                                                         lname){
            var filter;
            if (ns === "*" && lname === "*")
                filter = ftrue;
            else if (ns === "*")
                filter = localNameElementFilter(lname);
            else if (lname === "*")
                filter = namespaceElementFilter(ns);
            else
                filter = namespaceLocalNameElementFilter(ns, lname);

            return new impl.FilteredElementList(this, filter);
        }),

        getElementsByClassName: constant(function getElementsByClassName(names){
            names = names.trim();
            if (names === "") {
                var result = []; // Empty node list
                result._idlName = "NodeList";
                return result;
            }
            names = names.split(/\s+/);  // Split on spaces
            return new impl.FilteredElementList(this,
                                                classNamesElementFilter(names));
        }),

        getElementsByName: constant(function getElementsByName(name) {
            return new impl.FilteredElementList(this, elementNameFilter(name));
        }),

        adoptNode: constant(function adoptNode(node) {
            if (node.nodeType === DOCUMENT_NODE ||
                node.nodeType === DOCUMENT_TYPE_NODE) NotSupportedError();

            if (node.parentNode) node.parentNode.removeChild(node)

            if (node.ownerDocument !== this)
                recursivelySetOwner(node, this);

            return node;
        }),

        importNode: constant(function importNode(node, deep) {
            return this.adoptNode(node.cloneNode());
        }),

        // The following attributes and methods are from the HTML spec
        URL: attribute(nyi),
        domain: attribute(nyi, nyi),
        referrer: attribute(nyi),
        cookie: attribute(nyi, nyi),
        lastModified: attribute(nyi),
        // XXX Temporary hack
        readyState: attribute(function() { return "complete" }),
        title: attribute(fnoop, nyi),
        dir:  attribute(nyi, nyi),
        // Return the first <body> child of the document element.
        // XXX For now, setting this attribute is not implemented.
        body: attribute(function() {
            if (this.isHTML && this.documentElement) {
                var kids = this.documentElement.childNodes;
                for(var i = 0, n = kids.length; i < n; i++) {
                    if (kids[i].nodeType === ELEMENT_NODE &&
                        kids[i].localName === "body" &&
                        kids[i].namespaceURI === HTML_NAMESPACE) {
                        return kids[i];
                    }
                }
            }
            return null;
        }, nyi),
        // Return the first <head> child of the document element.
        head: attribute(function() {
            if (this.isHTML && this.documentElement) {
                var kids = this.documentElement.childNodes;
                for(var i = 0, n = kids.length; i < n; i++) {
                    if (kids[i].nodeType === ELEMENT_NODE &&
                        kids[i].localName === "head" &&
                        kids[i].namespaceURI === HTML_NAMESPACE) {
                        return kids[i];
                    }
                }
            }
            return null;
        }),
        images: attribute(nyi),
        embeds: attribute(nyi),
        plugins: attribute(nyi),
        links: attribute(nyi),
        forms: attribute(nyi),
        scripts: attribute(nyi),
        innerHTML: attribute(function() { return this.serialize() }, nyi),

        write: constant(function(args) {
            if (!this.isHTML) InvalidStateError();

            // XXX: still have to implement the ignore part
            if (!this._parser /* && this._ignore_destructive_writes > 0 */ )
                return;

            if (!this._parser) {
                // XXX call document.open, etc.
            }

            var s = join(arguments, "");

            // If the Document object's reload override flag is set, then
            // append the string consisting of the concatenation of all the
            // arguments to the method to the Document's reload override
            // buffer.
            // XXX: don't know what this is about.  Still have to do it

            // If there is no pending parsing-blocking script, have the
            // tokenizer process the characters that were inserted, one at a
            // time, processing resulting tokens as they are emitted, and
            // stopping when the tokenizer reaches the insertion point or when
            // the processing of the tokenizer is aborted by the tree
            // construction stage (this can happen if a script end tag token is
            // emitted by the tokenizer).

            // XXX: still have to do the above. Sounds as if we don't
            // always call parse() here.  If we're blocked, then we just
            // insert the text into the stream but don't parse it reentrantly...

            // Invoke the parser reentrantly
            this._parser.parse(s);
        }),

        writeln: constant(function writeln(args) {
            this.write(join(arguments, "") + "\n");
        }),

        // Utility methods
        clone: constant(function clone() {
            // Can't clone an entire document
            DataCloneError();
        }),
        isEqual: constant(function isEqual(n) {
            // Any two documents are shallowly equal.
            // Node.isEqualNode will also test the children
            return true;
        }),

        // Implementation-specific function.  Called when a text, comment,
        // or pi value changes.
        mutateValue: constant(function(node) {
            if (this.mutationHandler) {
                this.mutationHandler({
                    type: MUTATE_VALUE,
                    target: node._nid,
                    data: node.data
                });
            }
        }),

        // Invoked when an attribute's value changes. Attr holds the new
        // value.  oldval is the old value.  Attribute mutations can also
        // involve changes to the prefix (and therefore the qualified name)
        mutateAttr: constant(function(attr, oldval) {
            // Manage id->element mapping for getElementsById()
            // XXX: this special case id handling should not go here,
            // but in the attribute declaration for the id attribute
            /*
            if (attr.localName === "id" && attr.namespaceURI === null) {
                if (oldval) delId(oldval, attr.ownerElement);
                addId(attr.value, attr.ownerElement);
            }
            */
            if (this.mutationHandler) {
                this.mutationHandler({
                    type: MUTATE_ATTR,
                    target: attr.ownerElement._nid,
                    name: attr.localName,
                    ns: attr.namespaceURI,
                    value: attr.value,
                    prefix: attr.prefix
                });
            }
        }),

        // Used by removeAttribute and removeAttributeNS for attributes.
        mutateRemoveAttr: constant(function(attr) {
/*
 * This is now handled in Attributes.js
            // Manage id to element mapping
            if (attr.localName === "id" && attr.namespaceURI === null) {
                this.delId(attr.value, attr.ownerElement);
            }
*/
            if (this.mutationHandler) {
                this.mutationHandler({
                    type: MUTATE_REMOVE_ATTR,
                    target: attr.ownerElement._nid,
                    name: attr.localName,
                    ns: attr.namespaceURI
                });
            }
        }),

        // Called by Node.removeChild, etc. to remove a rooted element from
        // the tree. Only needs to generate a single mutation event when a
        // node is removed, but must recursively mark all descendants as not
        // rooted.
        mutateRemove: constant(function(node) {
            // Send a single mutation event
            if (this.mutationHandler) {
                this.mutationHandler({
                    type: MUTATE_REMOVE,
                    target: node._nid
                });
            }

            // Mark this and all descendants as not rooted
            recursivelyUproot(node);
        }),

        // Called when a new element becomes rooted.  It must recursively
        // generate mutation events for each of the children, and mark them all
        // as rooted.
        mutateInsert: constant(function(node) {
            // Mark node and its descendants as rooted
            recursivelyRoot(node);

            // Send a single mutation event
            if (this.mutationHandler) {
                this.mutationHandler({
                    type: MUTATE_INSERT,
                    target: node.parentNode._nid,
                    index: node.index,
                    nid: node._nid,
                    child: DOMSTR.serialize(node)
                });
            }
        }),

        // Called when a rooted element is moved within the document
        mutateMove: constant(function(node) {
            if (this.mutationHandler) {
                this.mutationHandler({
                    type: MUTATE_MOVE,
                    target: node._nid,
                    parent: node.parentNode._nid,
                    index: node.index
                });
            }
        }),


        // Add a mapping from  id to n for n.ownerDocument
        addId: constant(function addId(id, n) {
            var val = this.byId[id];
            if (!val) {
                this.byId[id] = n;
            }
            else {
                warn("Duplicate element id " + id);
                if (!isArray(val)) {
                    val = [val];
                    this.byId[id] = val;
                }
                val.push(n);
                sort(val, documentOrder);
            }
        }),

        // Delete the mapping from id to n for n.ownerDocument
        delId: constant(function delId(id, n) {
            var val = this.byId[id];
            assert(val);

            if (isArray(val)) {
                var idx = A.indexOf(val, n);
                splice(val, idx, 1);

                if (val.length == 1) { // convert back to a single node
                    this.byId[id] = val[0];
                }
            }
            else {
                delete this.byId[id];
            }
        }),

        _documentBaseURL: attribute(function() {
            // XXX: This is not implemented correctly yet
            return this._address;

            // The document base URL of a Document object is the
            // absolute URL obtained by running these substeps:

            //     Let fallback base url be the document's address.

            //     If fallback base url is about:blank, and the
            //     Document's browsing context has a creator browsing
            //     context, then let fallback base url be the document
            //     base URL of the creator Document instead.

            //     If the Document is an iframe srcdoc document, then
            //     let fallback base url be the document base URL of
            //     the Document's browsing context's browsing context
            //     container's Document instead.

            //     If there is no base element that has an href
            //     attribute, then the document base URL is fallback
            //     base url; abort these steps. Otherwise, let url be
            //     the value of the href attribute of the first such
            //     element.

            //     Resolve url relative to fallback base url (thus,
            //     the base href attribute isn't affected by xml:base
            //     attributes).

            //     The document base URL is the result of the previous
            //     step if it was successful; otherwise it is fallback
            //     base url.


        }),
    });

    var eventHandlerTypes = [
        "abort", "canplay", "canplaythrough", "change", "click", "contextmenu",
        "cuechange", "dblclick", "drag", "dragend", "dragenter", "dragleave",
        "dragover", "dragstart", "drop", "durationchange", "emptied", "ended",
        "input", "invalid", "keydown", "keypress", "keyup", "loadeddata",
        "loadedmetadata", "loadstart", "mousedown", "mousemove", "mouseout",
        "mouseover", "mouseup", "mousewheel", "pause", "play", "playing",
        "progress", "ratechange", "readystatechange", "reset", "seeked",
        "seeking", "select", "show", "stalled", "submit", "suspend",
        "timeupdate", "volumechange", "waiting",

        "blur", "error", "focus", "load", "scroll"
    ];

    // Add event handler idl attribute getters and setters to Document
    eventHandlerTypes.forEach(function(type) {
        // Define the event handler registration IDL attribute for this type
        Object.defineProperty(Document.prototype, "on" + type, {
            get: function() {
                return this._getEventHandler(type);
            },
            set: function(v) {
                this._setEventHandler(type, v);
            }
        });
    });



    function root(n) {
        n._nid = n.ownerDocument._nextnid++;
        n.ownerDocument._nodes[n._nid] = n;
        // Manage id to element mapping
        if (n.nodeType === ELEMENT_NODE) {
            var id = n.getAttribute("id");
            if (id) n.ownerDocument.addId(id, n);

            // Script elements need to know when they're inserted
            // into the document
            if (n._roothook) n._roothook();
        }
    }

    function uproot(n) {
        // Manage id to element mapping
        if (n.nodeType === ELEMENT_NODE) {
            var id = n.getAttribute("id");
            if (id) n.ownerDocument.delId(id, n);
        }
        delete n.ownerDocument._nodes[n._nid];
        delete n._nid;
    }

    function recursivelyRoot(node) {
        root(node);
        // XXX:
        // accessing childNodes on a leaf node creates a new array the
        // first time, so be careful to write this loop so that it
        // doesn't do that. node is polymorphic, so maybe this is hard to
        // optimize?  Try switching on nodeType?
/*
        if (node.hasChildNodes()) {
            var kids = node.childNodes;
            for(var i = 0, n = kids.length;  i < n; i++)
                recursivelyRoot(kids[i]);
        }
*/
        if (node.nodeType === ELEMENT_NODE) {
            var kids = node.childNodes;
            for(var i = 0, n = kids.length;  i < n; i++)
                recursivelyRoot(kids[i]);
        }
    }

    function recursivelyUproot(node) {
        uproot(node);
        for(var i = 0, n = node.childNodes.length;  i < n; i++)
            recursivelyUproot(node.childNodes[i]);
    }

    function recursivelySetOwner(node, owner) {
        node.ownerDocument = owner;
        delete node._lastModTime; // mod times are document-based
        var kids = node.childNodes;
        for(var i = 0, n = kids.length; i < n; i++)
            recursivelySetOwner(kids[i], owner);
    }


    // These functions return predicates for filtering elements.
    // They're used by the Document and Element classes for methods like
    // getElementsByTagName and getElementsByClassName

    function localNameElementFilter(lname) {
        return function(e) { return e.localName === lname; };
    }

    function htmlLocalNameElementFilter(lname) {
        var lclname = toLowerCase(lname);
        if (lclname === lname)
            return localNameElementFilter(lname);

        return function(e) {
            return e.isHTML
                ? e.localName === lclname
                : e.localName === lname;
        };
    }

    function namespaceElementFilter(ns) {
        return function(e) { return e.namespaceURI === ns; };
    }

    function namespaceLocalNameElementFilter(ns, lname) {
        return function(e) {
            return e.namespaceURI === ns && e.localName === lname;
        };
    }

    // XXX
    // Optimize this when I implement classList.
    function classNamesElementFilter(names) {
        return function(e) {
            var classAttr = e.getAttribute("class");
            if (!classAttr) return false;
            var classes = classAttr.trim().split(/\s+/);
            return every(names, function(n) {
                return A.indexOf(classes, n) !== -1;
            })
        }
    }

    function elementNameFilter(name) {
        return function(e) {
            return e.getAttribute("name") === name;
        }
    }


    return Document;
});



/************************************************************************
 *  src/impl/DocumentFragment.js
 ************************************************************************/

//@line 1 "src/impl/DocumentFragment.js"
defineLazyProperty(impl, "DocumentFragment", function() {
    function DocumentFragment(doc) {
        this.nodeType = DOCUMENT_FRAGMENT_NODE;
        this.ownerDocument = doc;
        this.childNodes = [];
        this.childNodes._idlName = "NodeList";
    }

    DocumentFragment.prototype = O.create(impl.Node.prototype, {
        _idlName: constant("DocumentFragment"),
//        nodeType: constant(DOCUMENT_FRAGMENT_NODE),
        nodeName: constant("#document-fragment"),
        nodeValue: attribute(fnull, fnoop),
        // Copy the text content getter/setter from Element
        textContent: O.getOwnPropertyDescriptor(impl.Element.prototype,
                                                "textContent"),

        // Utility methods
        clone: constant(function clone() {
            return new DocumentFragment(this.ownerDocument);
        }),
        isEqual: constant(function isEqual(n) {
            // Any two document fragments are shallowly equal.
            // Node.isEqualNode() will test their children for equality
            return true;
        }),

    });

    return DocumentFragment;
});


/************************************************************************
 *  src/impl/DocumentType.js
 ************************************************************************/

//@line 1 "src/impl/DocumentType.js"
defineLazyProperty(impl, "DocumentType", function() {
    function DocumentType(name, publicId, systemId) {
        // Unlike other nodes, doctype nodes always start off unowned
        // until inserted
        this.nodeType = DOCUMENT_TYPE_NODE;
        this.ownerDocument = null;
        this.name = name;
        this.publicId = publicId || "";
        this.systemId = systemId || "";
    }

    DocumentType.prototype = O.create(impl.Leaf.prototype, {
        _idlName: constant("DocumentType"),
//        nodeType: constant(DOCUMENT_TYPE_NODE),
        nodeName: attribute(function() { return this.name; }),
        nodeValue: attribute(fnull, fnoop),

        // Utility methods
        clone: constant(function clone() {
            DataCloneError();
        }),
        isEqual: constant(function isEqual(n) {
            return this.name === n.name &&
                this.publicId === n.publicId &&
                this.systemId === n.systemId;
        })
    });

    return DocumentType;
});


/************************************************************************
 *  src/impl/DOMImplementation.js
 ************************************************************************/

//@line 1 "src/impl/DOMImplementation.js"
defineLazyProperty(impl, "DOMImplementation", function() {
    // Each document must have its own instance of the domimplementation object
    // Even though these objects have no state
    function DOMImplementation() {};


    // Feature/version pairs that DOMImplementation.hasFeature() returns
    // true for.  It returns false for anything else.
    const supportedFeatures = {
        "xml": { "": true, "1.0": true, "2.0": true },   // DOM Core
        "core": { "": true, "2.0": true },               // DOM Core
        "html": { "": true, "1.0": true, "2.0": true} ,  // HTML
        "xhtml": { "": true, "1.0": true, "2.0": true} , // HTML
    };

    DOMImplementation.prototype = {
        _idlName: "DOMImplementation",
        hasFeature: function hasFeature(feature, version) {
            // Warning text directly modified slightly from the DOM Core spec:
            warn("Authors are strongly discouraged from using " +
                 "DOMImplementation.hasFeature(), as it is notoriously " +
                 "unreliable and imprecise. " +
                 "Use explicit feature testing instead.");

            var f = supportedFeatures[feature.toLowerCase()];

            return (f && f[version]) || false;
        },

        createDocumentType: function createDocumentType(qualifiedName,
                                                        publicId, systemId) {
            if (!xml.isValidName(qualifiedName)) InvalidCharacterError();
            if (!xml.isValidQName(qualifiedName)) NamespaceError();

            return new impl.DocumentType(qualifiedName, publicId, systemId);
        },

        createDocument: function createDocument(namespace,
                                                qualifiedName, doctype) {
            //
            // Note that the current DOMCore spec makes it impossible to
            // create an HTML document with this function, even if the
            // namespace and doctype are propertly set.  See this thread:
            // http://lists.w3.org/Archives/Public/www-dom/2011AprJun/0132.html
            //
            var address = null;
            if (currentlyExecutingScript)
                address = currentlyExecutingScript.ownerDocument._address
            var d = new impl.Document(false, address);
            var e;

            if (qualifiedName)
                e = d.createElementNS(namespace, qualifiedName);
            else
                e = null;

            if (doctype) {
                if (doctype.ownerDocument) WrongDocumentError();
                d.appendChild(doctype);
            }

            if (e) d.appendChild(e);

            return d;
        },

        createHTMLDocument: function createHTMLDocument(titleText) {
            var address = null;
            if (currentlyExecutingScript)
                address = currentlyExecutingScript.ownerDocument._address
            var d = new impl.Document(true, address);
            d.appendChild(new impl.DocumentType("html"));
            var html = d.createElement("html");
            d.appendChild(html);
            var head = d.createElement("head");
            html.appendChild(head);
            var title = d.createElement("title");
            head.appendChild(title);
            title.appendChild(d.createTextNode(titleText));
            html.appendChild(d.createElement("body"));
            return d;
        },


        mozSetOutputMutationHandler: function(doc, handler) {
            doc.mutationHandler = handler;
        },

        mozGetInputMutationHandler: function(doc) {
            nyi();
        },

        mozHTMLParser: HTMLParser,
    };

    return DOMImplementation;
});


/************************************************************************
 *  src/impl/FilteredElementList.js
 ************************************************************************/

//@line 1 "src/impl/FilteredElementList.js"
//
// This file defines node list implementation that lazily traverses
// the document tree (or a subtree rooted at any element) and includes
// only those elements for which a specified filter function returns true.
// It is used to implement the
// {Document,Element}.getElementsBy{TagName,ClassName}{,NS} methods.
//
defineLazyProperty(impl, "FilteredElementList", function() {
    function FilteredElementList(root, filter) {
        this.root = root;
        this.filter = filter;
        this.lastModTime = root.lastModTime
        this.done = false;
        this.cache = [];
    }

    FilteredElementList.prototype = {
        _idlName: "NodeList",

        get length() {
            this.checkcache();
            if (!this.done) this.traverse();
            return this.cache.length;
        },

        item: function(n) {
            this.checkcache();
            if (!this.done && n >= this.cache.length)
                this.traverse(n);
            return this.cache[n];
        },

        checkcache: function() {
            if (this.lastModTime !== this.root.lastModTime) {
                // subtree has changed, so invalidate cache
                this.cache.length = 0;
                this.done = false;
                this.lastModTime = this.root.lastModTime;
            }
        },

        // If n is specified, then traverse the tree until we've found the nth
        // item (or until we've found all items).  If n is not specified,
        // traverse until we've found all items.
        traverse: function(n) {
            // increment n so we can compare to length, and so it is never falsy
            if (n !== undefined) n++;

            var elt;
            while(elt = this.next()) {
                push(this.cache, elt);
                if (n && this.cache.length === n) return;
            }

            // no next element, so we've found everything
            this.done = true;
        },

        // Return the next element under root that matches filter
        next: function() {
            var start = (this.cache.length == 0)    // Start at the root or at
                ? this.root                         // the last element we found
                : this.cache[this.cache.length-1];

            var elt;
            if (start.nodeType === DOCUMENT_NODE)
                elt = start.documentElement;
            else
                elt = start.nextElement(this.root);

            while(elt) {
                if (this.filter(elt)) {
                    return elt;
                }

                elt = elt.nextElement(this.root);
            }
            return null;
        }
    };

    return FilteredElementList;
});


/************************************************************************
 *  src/impl/Event.js
 ************************************************************************/

//@line 1 "src/impl/Event.js"
defineLazyProperty(impl, "Event", function() {
    function Event(type, dictionary) {
        // Initialize basic event properties
        this.type = "";
        this.target = null;
        this.currentTarget = null;
        this.eventPhase = AT_TARGET;
        this.bubbles = false;
        this.cancelable = false;
        this.isTrusted = false;
        this.defaultPrevented = false;
        this.timeStamp = Date.now();

        // Initialize internal flags
        // XXX: Would it be better to inherit these defaults from the prototype?
        this._propagationStopped = false;
        this._immediatePropagationStopped = false;
        this._initialized = true;
        this._dispatching = false;

        // Now initialize based on the constructor arguments (if any)
        if (type) this.type = type;
        if (dictionary) {
            for(var p in dictionary)
                this[p] = dictionary[p];
        }
    }

    Event.prototype = O.create(Object.prototype, {
        _idlName: constant("Event"),
        stopPropagation: constant(function stopPropagation() {
            this._propagationStopped = true;
        }),

        stopImmediatePropagation: constant(function stopImmediatePropagation() {
            this._propagationStopped = true;
            this._immediatePropagationStopped = true;
        }),

        preventDefault: constant(function preventDefault() {
            if (this.cancelable)
                this.defaultPrevented = true;
        }),

        initEvent: constant(function initEvent(type, bubbles, cancelable) {
            this._initialized = true;
            if (this._dispatching) return;

            this._propagationStopped = false;
            this._immediatePropagationStopped = false;
            this.defaultPrevented = false;
            this.isTrusted = false;

            this.target = null;
            this.type = type;
            this.bubbles = bubbles;
            this.cancelable = cancelable;
        }),

    });

    return Event;
});



/************************************************************************
 *  src/impl/CustomEvent.js
 ************************************************************************/

//@line 1 "src/impl/CustomEvent.js"
defineLazyProperty(impl, "CustomEvent", function() {
    function CustomEvent(type, dictionary) {
        // Just use the superclass constructor to initialize
        impl.Event.call(this, type, dictionary);
    }
    CustomEvent.prototype = O.create(impl.Event.prototype, {
        _idlName: constant("CustomEvent"),
    });
    return CustomEvent;
});



/************************************************************************
 *  src/impl/UIEvent.js
 ************************************************************************/

//@line 1 "src/impl/UIEvent.js"
defineLazyProperty(impl, "UIEvent", function() {
    function UIEvent() {
        // Just use the superclass constructor to initialize
        impl.Event.call(this);

        this.view = null;  // FF uses the current window
        this.detail = 0;
    }
    UIEvent.prototype = O.create(impl.Event.prototype, {
        _idlName: constant("UIEvent"),
        initUIEvent: constant(function(type, bubbles, cancelable,
                                       view, detail) {
            this.initEvent(type, bubbles, cancelable);
            this.view = view;
            this.detail = detail;
        }),
    });
    return UIEvent;
});



/************************************************************************
 *  src/impl/MouseEvent.js
 ************************************************************************/

//@line 1 "src/impl/MouseEvent.js"
defineLazyProperty(impl, "MouseEvent", function() {
    function MouseEvent() {
        // Just use the superclass constructor to initialize
        impl.UIEvent.call(this);

        this.screenX = this.screenY = this.clientX = this.clientY = 0;
        this.ctrlKey = this.altKey = this.shiftKey = this.metaKey = false;
        this.button = 0;
        this.buttons = 1;
        this.relatedTarget = null;
    }
    MouseEvent.prototype = O.create(impl.UIEvent.prototype, {
        _idlName: constant("MouseEvent"),
        initMouseEvent: constant(function(type, bubbles, cancelable,
                                          view, detail,
                                          screenX, screenY, clientX, clientY,
                                          ctrlKey, altKey, shiftKey, metaKey,
                                          button, relatedTarget) {
            this.initEvent(type, bubbles, cancelable, view, detail);
            this.screenX = screenX;
            this.screenY = screenY;
            this.clientX = clientX;
            this.clientY = clientY;
            this.ctrlKey = ctrlKey;
            this.altKey = altKey;
            this.shiftKey = shiftKey;
            this.metaKey = metaKey;
            this.button = button;
            switch(button) {
            case 0: this.buttons = 1; break;
            case 1: this.buttons = 4; break;
            case 2: this.buttons = 2; break;
            default: this.buttons = 0; break;
            }
            this.relatedTarget = relatedTarget;
        }),

        getModifierState: constant(function(key) {
            switch(key) {
            case "Alt": return this.altKey;
            case "Control": return this.ctrlKey;
            case "Shift": return this.shiftKey;
            case "Meta": return this.metaKey;
            default: return false;
            }
        }),
    });

    return MouseEvent;
});



/************************************************************************
 *  src/impl/WheelEvent.js
 ************************************************************************/

//@line 1 "src/impl/WheelEvent.js"
defineLazyProperty(impl, "WheelEvent", function() {
    function WheelEvent() {
        // Just use the superclass constructor to initialize
        impl.MouseEvent.call(this);

        this.screenX = this.screenY = this.clientX = this.clientY = 0;
        this.ctrlKey = this.altKey = this.shiftKey = this.metaKey = false;
        this.deltaX = this.deltaY = this.deltaZ = 0;
        this.button = 0;
        this.buttons = 1;
        this.relatedTarget = null;
    }
    WheelEvent.prototype = O.create(impl.MouseEvent.prototype, {
        _idlName: constant("WheelEvent"),
        initWheelEvent: constant(function(type, bubbles, cancelable,
                                          view, detail,
                                          screenX, screenY, clientX, clientY, deltaX, deltaY, deltaZ,
                                          ctrlKey, altKey, shiftKey, metaKey,
                                          button, relatedTarget) {
            
            this.initMouseEvent(type, bubbles, cancelable, view, detail, screenX, screenY, clientX, clientY,
                ctrlKey, altKey, shiftKey, metaKey, button, relatedTarget);
            
            this.deltaX = deltaX;
            this.deltaY = deltaY;
            this.deltaZ = deltaZ;
        }),
    });

    return WheelEvent;
});



/************************************************************************
 *  src/impl/KeyboardEvent.js
 ************************************************************************/

//@line 1 "src/impl/KeyboardEvent.js"
defineLazyProperty(impl, "KeyboardEvent", function() {
    function KeyboardEvent() {
        // Just use the superclass constructor to initialize
        impl.UIEvent.call(this);

        this.key = "\0";
        this.keyCode = 0;
    }
    KeyboardEvent.prototype = O.create(impl.UIEvent.prototype, {
        _idlName: constant("KeyboardEvent"),
        initKeyboardEvent: constant(function(type, key, bubbles, cancelable,
                                          view, detail) {
            this.initEvent(type, bubbles, cancelable, view, detail);
            this.key = key;
            this.keyCode = key;
        }),

    });

    return KeyboardEvent;
});



/************************************************************************
 *  src/impl/VoiceEvent.js
 ************************************************************************/

//@line 1 "src/impl/VoiceEvent.js"
defineLazyProperty(impl, "VoiceEvent", function() {
    function VoiceEvent() {
        // Just use the superclass constructor to initialize
        impl.UIEvent.call(this);

        this.command = "";
        this.type = "";
        this.confidence = 0;
    }
    VoiceEvent.prototype = O.create(impl.UIEvent.prototype, {
        _idlName: constant("VoiceEvent"),
        initVoiceEvent: constant(function(type, command, confidence, bubbles, cancelable,
                                          view, detail) {
            this.initEvent(type, bubbles, cancelable, view, detail);
            this.command = command;
            this.confidence = confidence;
            this.type = type;
        }),

    });

    return VoiceEvent;
});



/************************************************************************
 *  src/impl/SpatialInputEvent.js
 ************************************************************************/

//@line 1 "src/impl/SpatialInputEvent.js"
defineLazyProperty(impl, "SpatialInputEvent", function() {
    function SpatialInputEvent() {
        // Just use the superclass constructor to initialize
        impl.UIEvent.call(this);

        this.isPressed = false;
        this.x = 0;
        this.y = 0;
        this.z = 0;
        this.sourceKind = 0;
    }
    SpatialInputEvent.prototype = O.create(impl.UIEvent.prototype, {
        _idlName: constant("SpatialInputEvent"),
        initSpatialInputEvent: constant(function(type, isPressedArg, xArg,  yArg, zArg, sourceKindArg, bubbles, cancelable,
                                          view, detail) {
            this.initEvent(type, bubbles, cancelable, view, detail);
            this.isPressed = isPressedArg;
            this.x = xArg;
            this.y = yArg;
            this.z = zArg;
            this.sourceKind = sourceKindArg;
        }),

    });

    return SpatialInputEvent;
});



/************************************************************************
 *  src/impl/HTMLElement.js
 ************************************************************************/

//@line 1 "src/impl/HTMLElement.js"
var tagNameToInterfaceName = {
    "a": "HTMLAnchorElement",
    "abbr": "HTMLElement",
    "address": "HTMLElement",
    "area": "HTMLAreaElement",
    "article": "HTMLElement",
    "aside": "HTMLElement",
    "audio": "HTMLAudioElement",
    "b": "HTMLElement",
    "base": "HTMLBaseElement",
    "bdi": "HTMLElement",
    "bdo": "HTMLElement",
    "blockquote": "HTMLQuoteElement",
    "body": "HTMLBodyElement",
    "br": "HTMLBRElement",
    "button": "HTMLButtonElement",
    "canvas": "HTMLCanvasElement",
    "exp-holo-canvas": "HTMLHoloCanvasElementExp",
    "caption": "HTMLTableCaptionElement",
    "cite": "HTMLElement",
    "code": "HTMLElement",
    "col": "HTMLTableColElement",
    "colgroup": "HTMLTableColElement",
    "command": "HTMLCommandElement",
    "datalist": "HTMLDataListElement",
    "dd": "HTMLElement",
    "del": "HTMLModElement",
    "details": "HTMLDetailsElement",
    "dfn": "HTMLElement",
    "div": "HTMLDivElement",
    "dl": "HTMLDListElement",
    "dt": "HTMLElement",
    "em": "HTMLElement",
    "embed": "HTMLEmbedElement",
    "fieldset": "HTMLFieldSetElement",
    "figcaption": "HTMLElement",
    "figure": "HTMLElement",
    "footer": "HTMLElement",
    "form": "HTMLFormElement",
    "frame": "HTMLFrameElement",
    "frameset": "HTMLFrameSetElement",
    "h1": "HTMLHeadingElement",
    "h2": "HTMLHeadingElement",
    "h3": "HTMLHeadingElement",
    "h4": "HTMLHeadingElement",
    "h5": "HTMLHeadingElement",
    "h6": "HTMLHeadingElement",
    "head": "HTMLHeadElement",
    "header": "HTMLElement",
    "hgroup": "HTMLElement",
    "hr": "HTMLHRElement",
    "html": "HTMLHtmlElement",
    "i": "HTMLElement",
    "iframe": "HTMLIFrameElement",
    "img": "HTMLImageElement",
    "input": "HTMLInputElement",
    "ins": "HTMLModElement",
    "kbd": "HTMLElement",
    "keygen": "HTMLKeygenElement",
    "label": "HTMLLabelElement",
    "legend": "HTMLLegendElement",
    "li": "HTMLLIElement",
    "link": "HTMLLinkElement",
    "map": "HTMLMapElement",
    "mark": "HTMLElement",
    "menu": "HTMLMenuElement",
    "meta": "HTMLMetaElement",
    "meter": "HTMLMeterElement",
    "nav": "HTMLElement",
    "noscript": "HTMLElement",
    "object": "HTMLObjectElement",
    "ol": "HTMLOListElement",
    "optgroup": "HTMLOptGroupElement",
    "option": "HTMLOptionElement",
    "output": "HTMLOutputElement",
    "p": "HTMLParagraphElement",
    "param": "HTMLParamElement",
    "pre": "HTMLPreElement",
    "progress": "HTMLProgressElement",
    "q": "HTMLQuoteElement",
    "rp": "HTMLElement",
    "rt": "HTMLElement",
    "ruby": "HTMLElement",
    "s": "HTMLElement",
    "samp": "HTMLElement",
    "script": "HTMLScriptElement",
    "section": "HTMLElement",
    "select": "HTMLSelectElement",
    "small": "HTMLElement",
    "source": "HTMLSourceElement",
    "span": "HTMLSpanElement",
    "strong": "HTMLElement",
    "style": "HTMLStyleElement",
    "sub": "HTMLElement",
    "summary": "HTMLElement",
    "sup": "HTMLElement",
    "table": "HTMLTableElement",
    "tbody": "HTMLTableSectionElement",
    "td": "HTMLTableDataCellElement",
    "textarea": "HTMLTextAreaElement",
    "tfoot": "HTMLTableSectionElement",
    "th": "HTMLTableHeaderCellElement",
    "thead": "HTMLTableSectionElement",
    "time": "HTMLTimeElement",
    "title": "HTMLTitleElement",
    "tr": "HTMLTableRowElement",
    "track": "HTMLTrackElement",
    "u": "HTMLElement",
    "ul": "HTMLUListElement",
    "var": "HTMLElement",
    "video": "HTMLVideoElement",
    "wbr": "HTMLElement",
};

defineLazyProperty(impl, "HTMLElement", function() {
    function HTMLElement(doc, localName, prefix) {
        impl.Element.call(this, doc, localName, HTML_NAMESPACE, prefix);
    }

    HTMLElement.prototype = O.create(impl.Element.prototype, {
        _idlName: constant("HTMLElement"),
        innerHTML: attribute(
            function() {
                return this.serialize();
            },
            function(v) {
                var parser = this.ownerDocument.implementation.mozHTMLParser(
                    this.ownerDocument._address,
                    this);
                parser.parse(v, true);
                var tmpdoc = parser.document();
                var root = tmpdoc.firstChild;

                // Remove any existing children of this node
                while(this.hasChildNodes())
                    this.removeChild(this.firstChild);

                // Now copy newly parsed children from the root to this node
                while(root.hasChildNodes()) {
                    this.appendChild(root.firstChild);
                }
            }),
        style: attribute(function() {
            if (!this._style)
                this._style = new impl.CSSStyleDeclaration(this);
            return this._style;
        }),

        click: constant(function() {
            if (this._click_in_progress) return;
            this._click_in_progress = true;
            try {
                if (this._pre_click_activation_steps)
                    this._pre_click_activation_steps();

                var event = this.ownerDocument.createEvent("MouseEvent");
                event.initMouseEvent("click", true, true,
                                     this.ownerDocument.defaultView, 1,
                                     0, 0, 0, 0,
                                     // These 4 should be initialized with
                                     // the actually current keyboard state
                                     // somehow...
                                     false, false, false, false,
                                     0, null)

                // Dispatch this as an untrusted event since it is synthetic
                var success = this.dispatchEvent(event);

                if (success) {
                    if (this._post_click_activation_steps)
                        this._post_click_activation_steps(event);
                }
                else {
                    if (this._cancelled_activation_steps)
                        this._cancelled_activation_steps();
                }
            }
            finally {
                this._click_in_progress = false;
            }
        }),
    });

    impl.Element.reflectStringAttribute(HTMLElement, "title");
    impl.Element.reflectStringAttribute(HTMLElement, "lang");
    impl.Element.reflectEnumeratedAttribute(HTMLElement, "dir", null, {
        ltr: "ltr",
        rtl: "rtl",
        auto:"auto"
    });

    impl.Element.reflectStringAttribute(HTMLElement, "accesskey", "accessKey");
    impl.Element.reflectBooleanAttribute(HTMLElement, "hidden");

    // XXX: the default value for tabIndex should be 0 if the element is
    // focusable and -1 if it is not.  But the full definition of focusable
    // is actually hard to compute, so for now, I'll follow Firefox and
    // just base the default value on the type of the element.
    var focusableElements = {
        "A":true, "LINK":true, "BUTTON":true, "INPUT":true,
        "SELECT":true, "TEXTAREA":true, "COMMAND":true
    };
    impl.Element.reflectIntegerAttribute(HTMLElement, "tabindex",
                         // compute a default tabIndex value
                         function() {
                             if (this.tagName in focusableElements ||
                                 this.contentEditable)
                                 return 0;
                             else
                                 return -1;
                         },
                                         "tabIndex");

    // XXX: reflect contextmenu as contextMenu, with element type


    // style: the spec doesn't call this a reflected attribute.
    //   may want to handle it manually.

    // contentEditable: enumerated, not clear if it is actually
    // reflected or requires custom getter/setter. Not listed as
    // "limited to known values".  Raises syntax_err on bad setting,
    // so I think this is custom.

    // contextmenu: content is element id, idl type is an element
    // draggable: boolean, but not a reflected attribute
    // dropzone: reflected SettableTokenList, experimental, so don't
    //   implement it right away.

    // data-* attributes: need special handling in setAttribute?
    // Or maybe that isn't necessary. Can I just scan the attribute list
    // when building the dataset?  Liveness and caching issues?

    // microdata attributes: many are simple reflected attributes, but
    // I'm not going to implement this now.


    var eventHandlerTypes = [
        "abort", "canplay", "canplaythrough", "change", "click", "contextmenu",
        "cuechange", "dblclick", "drag", "dragend", "dragenter", "dragleave",
        "dragover", "dragstart", "drop", "durationchange", "emptied", "ended",
        "input", "invalid", "keydown", "keypress", "keyup", "loadeddata",
        "loadedmetadata", "loadstart", "mousedown", "mousemove", "mouseout",
        "mouseover", "mouseup", "mousewheel", "pause", "play", "playing",
        "progress", "ratechange", "readystatechange", "reset", "seeked",
        "seeking", "select", "show", "stalled", "submit", "suspend",
        "timeupdate", "volumechange", "waiting",

        // These last 5 event types will be overriden by HTMLBodyElement
        "blur", "error", "focus", "load", "scroll"
    ];

    eventHandlerTypes.forEach(function(type) {
        // Define the event handler registration IDL attribute for this type
        Object.defineProperty(HTMLElement.prototype, "on" + type, {
            get: function() {
                return this._getEventHandler(type);
            },
            set: function(v) {
                this._setEventHandler(type, v);
            },
        });

        function EventHandlerChangeHandler(elt, name, oldval, newval) {
            var doc = elt.ownerDocument ? wrap(elt.ownerDocument) : {};
            var form = elt.form ? wrap(elt.form) : {};
            var element = wrap(elt);

            // EventHandlerBuilder uses with, so it is in src/loose.js
            elt[name] = new EventHandlerBuilder(newval,
                                                doc, form, element).build();
        }

        // Define special behavior for the content attribute as well
        impl.Element.registerAttributeChangeHandler(HTMLElement, "on" + type,
                                                    EventHandlerChangeHandler);
    });

    return HTMLElement;
});

defineLazyProperty(impl, "HTMLAnchorElement", function() {
    function HTMLAnchorElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLAnchorElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLAnchorElement"),

        _post_click_activation_steps: constant(function(e) {
            if (this.href) {
                // Follow the link
                // XXX: this is just a quick hack
                // XXX: the HTML spec probably requires more than this
                this.ownerDocument.defaultView.location = this.href;
            }
        })
    });

    // XXX impl.Element.reflectURLAttribute(HTMLAnchorElement, "href");
    // XXX impl.Element.reflectURLAttribute(HTMLAnchorElement, "ping");
    impl.Element.reflectStringAttribute(HTMLAnchorElement, "download");
    impl.Element.reflectStringAttribute(HTMLAnchorElement, "target");
    impl.Element.reflectStringAttribute(HTMLAnchorElement, "rel");
    impl.Element.reflectStringAttribute(HTMLAnchorElement, "media");
    impl.Element.reflectStringAttribute(HTMLAnchorElement, "hreflang");
    impl.Element.reflectStringAttribute(HTMLAnchorElement, "type");
    // XXX: also reflect relList

    return HTMLAnchorElement;
});

defineLazyProperty(impl, "HTMLAreaElement", function() {
    function HTMLAreaElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLAreaElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLAreaElement"),
    });

    // XXX impl.Element.reflectURLAttribute(HTMLAreaElement, "href");
    // XXX impl.Element.reflectURLAttribute(HTMLAreaElement, "ping");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "alt");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "target");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "download");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "rel");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "media");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "hreflang");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "type");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "shape");
    impl.Element.reflectStringAttribute(HTMLAreaElement, "coords");
    // XXX: also reflect relList

    return HTMLAreaElement;
});

defineLazyProperty(impl, "HTMLBRElement", function() {
    function HTMLBRElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLBRElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLBRElement"),
    });

    return HTMLBRElement;
});

defineLazyProperty(impl, "HTMLBaseElement", function() {
    function HTMLBaseElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLBaseElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLBaseElement"),
    });

    impl.Element.reflectStringAttribute(HTMLBaseElement, "target");

    return HTMLBaseElement;
});

defineLazyProperty(impl, "HTMLBodyElement", function() {
    function HTMLBodyElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLBodyElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLBodyElement"),
    });


    // Certain event handler attributes on a <body> tag actually set
    // handlers for the window rather than just that element.  Define
    // getters and setters for those here.  Note that some of these override
    // properties on HTMLElement.prototype.
    // XXX: If I add support for <frameset>, these have to go there, too
    // XXX
    // When the Window object is implemented, these attribute will have
    // to work with the same-named attributes on the Window.

    var eventHandlerTypes = [
        "afterprint", "beforeprint", "beforeunload", "blur", "error",
        "focus","hashchange", "load", "message", "offline", "online",
        "pagehide", "pageshow","popstate","resize","scroll","storage","unload",
    ];

    eventHandlerTypes.forEach(function(type) {
        // Define the event handler registration IDL attribute for this type
        Object.defineProperty(HTMLBodyElement.prototype, "on" + type, {
            get: function() {
                // XXX: read these from the Window object instead?
                return this._getEventHandler(type);
            },
            set: function(v) {
                // XXX: write to the Window object instead?
                this._setEventHandler(type, v);
            },
        });

        function EventHandlerChangeHandler(elt, name, oldval, newval) {
            var doc = elt.ownerDocument ? wrap(elt.ownerDocument) : {};
            var form = elt.form ? wrap(elt.form) : {};
            var element = wrap(elt);

            // EventHandlerBuilder uses with, so it is in src/loose.js
            elt[name] = new EventHandlerBuilder(newval,
                                                doc, form, element).build();
        }

        // Define special behavior for the content attribute as well
        impl.Element.registerAttributeChangeHandler(HTMLBodyElement,"on" + type,
                                                    EventHandlerChangeHandler);
    });


    return HTMLBodyElement;
});

defineLazyProperty(impl, "HTMLButtonElement", function() {
    function HTMLButtonElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLButtonElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLButtonElement"),
    });

    impl.Element.reflectStringAttribute(HTMLButtonElement, "name");
    impl.Element.reflectBooleanAttribute(HTMLButtonElement, "disabled");
    impl.Element.reflectBooleanAttribute(HTMLButtonElement, "autofocus");

    impl.Element.reflectStringAttribute(HTMLButtonElement, "value");
    impl.Element.reflectEnumeratedAttribute(HTMLButtonElement, "type", null, {
        submit: "submit",
        reset: "reset",
        button: "button",
    }, "submit");

    impl.Element.reflectStringAttribute(HTMLButtonElement,
                                        "formtarget", "formTarget");
    impl.Element.reflectBooleanAttribute(HTMLButtonElement,
                                         "formnovalidate", "formNoValidate");
    impl.Element.reflectEnumeratedAttribute(HTMLButtonElement,
                                            "formmethod", "formMethod", {
                                                get: "get",
                                                post: "post"
                                            }, "get");
    impl.Element.reflectEnumeratedAttribute(HTMLButtonElement,
                                            "formenctype", "formEnctype", {
        "application/x-www-form-urlencoded":"application/x-www-form-urlencoded",
        "multipart/form-data":"multipart/form-data",
        "text/plain": "text/plain"
    }, "application/x-www-form-urlencoded");


    return HTMLButtonElement;
});

defineLazyProperty(impl, "HTMLCanvasElement", function() {
    function HTMLCanvasElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
        this._width = 0;
        this._height = 0;

        this.context2D = new holographic.nativeInterface.makeRenderingContext();
        this.context2D._idlName = "RenderingContext";
    }

    HTMLCanvasElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLCanvasElement"),

        getContext: constant(function getContext(contextType) {
            if (contextType === '2d') {
                return this.context2D;
            } else {
                return null;
            }
        }),

        width: attribute(
            function () {
                return this._width;
            },
            function (value) {
                this._width = value;
                holographic.nativeInterface.canvas2d.setWidth(this.context2D.ctxNative, value);
            }
        ),

        height: attribute(
            function () {
                return this._height;
            },
            function (value) {
                this._height = value;
                holographic.nativeInterface.canvas2d.setHeight(this.context2D.ctxNative, value);
            }
        ),
    });

    return HTMLCanvasElement;
});

defineLazyProperty(impl, "HTMLHoloCanvasElementExp", function() {
    function HTMLHoloCanvasElementExp(doc, localName, prefix) {
        this.width = this.clientWidth = window.width;
        this.height = this.clientHeight = window.height;
        holographic.canvas = this;
        impl.HTMLElement.call(this, doc, localName, prefix);

        this.addEventListenerXXX = this.addEventListener;
        this.removeEventListenerXXX = this.removeEventListener;
        
        this.addEventListener = function (type, listener, capture) {
            holographic.nativeInterface.input.addEventListener(type);
            this.addEventListenerXXX(type, listener, capture);
        };
        
        this.removeEventListener = function (type, listener, capture) {
            holographic.nativeInterface.input.removeEventListener(type);
            this.removeEventListenerXXX(type, listener, capture);
        };

        // The order of these events must match the native implementation
        this.spatialInputEvents = ['sourcepress', 'sourcerelease', 'sourcelost', 'sourcedetected', 'sourceupdate'];
        this.keyboardEvents = ['keydown', 'keyup'];
        this.mouseEvents = ['mouseup', 'mousedown', 'mousemove', 'wheel'];
        this.voiceEvents = ['voicecommand'];
    }

    HTMLHoloCanvasElementExp.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLHoloCanvasElementExp"),

        getBoundingClientRect: constant(function getBoundingClientRect() {
            var rect = {};
            rect.top = 0;
            rect.bottom = this.height;

            rect.width = this.width;
            rect.height = this.height;

            rect.left = 0;
            rect.right = this.width;

            rect.x = 0;
            rect.y = 0;

            return rect;
        }),

        getContext: constant(function getContext(contextType) {
            if (contextType === 'experimental-webgl' || contextType === 'webgl') {
                if (this.context) {
                    return this.context;
                } else {
                    this.context = new holographic.nativeInterface.makeWebGLRenderingContext();
                    this.context._idlName = "RenderingContext";
                    return this.context;
                }
            } else {
                return null;
            }
        }),

        dispatchMouseFromWindow: constant(function dispatchMouseFromWindow(x, y, button, actionId, wheelDelta) {
            let event;
            let realButton = button;
            // Native button parameter is actually MouseEvent.buttons. Convert it to MouseEvent.button now
            if (button & 1) {
                realButton = 0;
            } else if (button & 2) {
                realButton = 2;
            } else if (button & 4) {
                realButton = 1;
            }

            let action = this.mouseEvents[actionId];
            
            if (action ===  "wheel") {
                event = this.ownerDocument.createEvent("WheelEvent");
                event.initWheelEvent(action, true, true,
                    this.ownerDocument.defaultView, 1,
                    x, y, x, y, 0, wheelDelta, 0,
                    // These 4 should be initialized with
                    // the actually current keyboard state
                    // somehow...
                    false, false, false, false,
                    realButton, null);
            } else {
                event = this.ownerDocument.createEvent("MouseEvent");
                event.initMouseEvent(action, true, true,
                    this.ownerDocument.defaultView, 1,
                    x, y, x, y,
                    // These 4 should be initialized with
                    // the actually current keyboard state
                    // somehow...
                    false, false, false, false,
                    realButton, null);
            }

            // Dispatch this as an untrusted event since it is synthetic
            this.dispatchEvent(event);
            return event;
        }),

        dispatchKeyboardFromWindow: constant(function dispatchKeyboardFromWindow(key, typeId) {
            let keyEvent = this.ownerDocument.createEvent("KeyboardEvent");
            keyEvent.initKeyboardEvent(this.keyboardEvents[typeId], key, true, true);
            this.dispatchEvent(keyEvent);
            return keyEvent;
        }),

        dispatchVoiceFromWindow: constant(function dispatchVoiceFromWindow(typeId, command, confidence ) {
            let voiceEvent = this.ownerDocument.createEvent("VoiceEvent");
            voiceEvent.initVoiceEvent(this.voiceEvents[typeId], command, confidence, true, true);
            this.dispatchEvent(voiceEvent);
            return voiceEvent;
        }),

        dispatchSpatialInputFromWindow: constant(function dispatchSpatialInputFromWindow(xArg,  yArg, zArg, isPressedArg, sourceKindArg, typeId) {
            let spatialInputEvent = this.ownerDocument.createEvent("SpatialInputEvent");
            spatialInputEvent.initSpatialInputEvent(this.spatialInputEvents[typeId], isPressedArg, xArg, yArg, zArg, sourceKindArg, true, true, )
            this.dispatchEvent(spatialInputEvent);
        }),
    });

    return HTMLHoloCanvasElementExp;
});

defineLazyProperty(impl, "HTMLCommandElement", function() {
    function HTMLCommandElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLCommandElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLCommandElement"),
    });

    impl.Element.reflectEnumeratedAttribute(HTMLCommandElement, "type", null,
                                            {
                                                command: "command",
                                                checkbox: "checkbox",
                                                radio: "radio"
                                            }, "command");
    impl.Element.reflectStringAttribute(HTMLCommandElement, "label");
    impl.Element.reflectBooleanAttribute(HTMLCommandElement, "disabled");
    impl.Element.reflectBooleanAttribute(HTMLCommandElement, "checked");
    impl.Element.reflectStringAttribute(HTMLCommandElement, "radiogroup");
    // XXX: also reflect URL attribute icon

    return HTMLCommandElement;
});

defineLazyProperty(impl, "HTMLDListElement", function() {
    function HTMLDListElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLDListElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLDListElement"),
    });

    return HTMLDListElement;
});

defineLazyProperty(impl, "HTMLDataListElement", function() {
    function HTMLDataListElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLDataListElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLDataListElement"),
    });

    return HTMLDataListElement;
});

defineLazyProperty(impl, "HTMLDetailsElement", function() {
    function HTMLDetailsElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLDetailsElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLDetailsElement"),
    });

    impl.Element.reflectBooleanAttribute(HTMLDetailsElement, "open");

    return HTMLDetailsElement;
});

defineLazyProperty(impl, "HTMLDivElement", function() {
    function HTMLDivElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLDivElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLDivElement"),
    });

    return HTMLDivElement;
});

defineLazyProperty(impl, "HTMLEmbedElement", function() {
    function HTMLEmbedElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLEmbedElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLEmbedElement"),
    });

    // XXX impl.Element.reflectURLAttribute(HTMLEmbedElement, "src");
    impl.Element.reflectStringAttribute(HTMLEmbedElement, "type");
    impl.Element.reflectStringAttribute(HTMLEmbedElement, "width");
    impl.Element.reflectStringAttribute(HTMLEmbedElement, "height");

    return HTMLEmbedElement;
});

defineLazyProperty(impl, "HTMLFieldSetElement", function() {
    function HTMLFieldSetElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLFieldSetElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLFieldSetElement"),
    });

    impl.Element.reflectBooleanAttribute(HTMLFieldSetElement, "disabled");
    impl.Element.reflectStringAttribute(HTMLFieldSetElement, "name");

    return HTMLFieldSetElement;
});

defineLazyProperty(impl, "HTMLFormElement", function() {
    function HTMLFormElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLFormElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLFormElement"),
    });

    impl.Element.reflectEnumeratedAttribute(HTMLFormElement, "autocomplete",
                                            null,
                                            {
                                                on: "on",
                                                off: "off"
                                            }, "on");
    impl.Element.reflectStringAttribute(HTMLFormElement, "name");
    impl.Element.reflectStringAttribute(HTMLFormElement,
                                        "accept-charset", "acceptCharset");

    impl.Element.reflectStringAttribute(HTMLFormElement, "target");
    impl.Element.reflectBooleanAttribute(HTMLFormElement,
                                         "novalidate", "noValidate");
    impl.Element.reflectEnumeratedAttribute(HTMLFormElement, "method", null, {
        get: "get",
        post: "post"
    }, "get");

    // Both enctype and encoding reflect the enctype content attribute
    impl.Element.reflectEnumeratedAttribute(HTMLFormElement, "enctype", null, {
        "application/x-www-form-urlencoded":"application/x-www-form-urlencoded",
        "multipart/form-data":"multipart/form-data",
        "text/plain": "text/plain"
    }, "application/x-www-form-urlencoded");
    impl.Element.reflectEnumeratedAttribute(HTMLFormElement,
                                            "enctype", "encoding", {
        "application/x-www-form-urlencoded":"application/x-www-form-urlencoded",
        "multipart/form-data":"multipart/form-data",
        "text/plain": "text/plain"
    }, "application/x-www-form-urlencoded");

    return HTMLFormElement;
});

defineLazyProperty(impl, "HTMLHRElement", function() {
    function HTMLHRElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLHRElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLHRElement"),
    });

    return HTMLHRElement;
});

defineLazyProperty(impl, "HTMLHeadElement", function() {
    function HTMLHeadElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLHeadElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLHeadElement"),
    });

    return HTMLHeadElement;
});

defineLazyProperty(impl, "HTMLHeadingElement", function() {
    function HTMLHeadingElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLHeadingElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLHeadingElement"),
    });

    return HTMLHeadingElement;
});

defineLazyProperty(impl, "HTMLHtmlElement", function() {
    function HTMLHtmlElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLHtmlElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLHtmlElement"),
    });

    return HTMLHtmlElement;
});

defineLazyProperty(impl, "HTMLIFrameElement", function() {
    function HTMLIFrameElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLIFrameElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLIFrameElement"),
    });

    // XXX impl.reflectURLAttribute(HTMLIFrameElement, "src");
    impl.Element.reflectStringAttribute(HTMLIFrameElement, "srcdoc");
    impl.Element.reflectStringAttribute(HTMLIFrameElement, "name");
    impl.Element.reflectStringAttribute(HTMLIFrameElement, "width");
    impl.Element.reflectStringAttribute(HTMLIFrameElement, "height");
    // XXX: sandbox is a reflected settable token list
    impl.Element.reflectBooleanAttribute(HTMLIFrameElement, "seamless");

    return HTMLIFrameElement;
});

defineLazyProperty(impl, "HTMLImageElement", function() {
    function HTMLImageElement(doc, localName, prefix) {
        this.native = new holographic.nativeInterface.image.createImage();

        this.nativeCallback = function(type) {
            if (type === 'load' && arguments.length > 2) {
                this.width = arguments[1];
                this.height = arguments[2];
                holographic.nativeInterface.eventing.removeCallback(this.native);

                var loadEvent = this.ownerDocument.createEvent("Event");
                loadEvent.initEvent("load", true, true);
                this.dispatchEvent(loadEvent);
            }
        };

        Object.defineProperty(this, "src", {
            set: function setSrc(value) {
                this.source = value;
                var isBlob = false;
                for (var index = 0; index < URL.objects.length; index++) {
                    if (value === URL.objects[index].key) {
                        holographic.nativeInterface.image.setImageSourceFromBlob(this.native, URL.objects[index].data);
                        holographic.nativeInterface.eventing.setCallback(this.native, this.nativeCallback.bind(this));
                        isBlob = true;
                        break;
                    }
                }
                if (!isBlob) {
                    holographic.nativeInterface.image.setImageSource(this.native, value);
                    holographic.nativeInterface.eventing.setCallback(this.native, this.nativeCallback.bind(this));
                }
            },
            get: function getSrc() {
                return this.source;
            }
        });

        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLImageElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLImageElement"),

        getData: constant(function getData() {
            return this.native;      
        }),

    });

    // XXX: I don't know whether to reflect crossorigin as a string or
    // as an enumerated attribute. Since it is not "limited to only
    // known values", I think it is just a string
    impl.Element.reflectStringAttribute(HTMLImageElement, "alt");
    impl.Element.reflectStringAttribute(HTMLImageElement, "crossorigin",
                                        "crossOrigin");
    impl.Element.reflectStringAttribute(HTMLImageElement, "usemap", "useMap");
    impl.Element.reflectBooleanAttribute(HTMLImageElement, "ismap", "isMap");



    return HTMLImageElement;
});

defineLazyProperty(impl, "HTMLInputElement", function() {
    function HTMLInputElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLInputElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLInputElement"),
    });


    impl.Element.reflectStringAttribute(HTMLInputElement, "name");
    impl.Element.reflectBooleanAttribute(HTMLInputElement, "disabled");
    impl.Element.reflectBooleanAttribute(HTMLInputElement, "autofocus");

    impl.Element.reflectStringAttribute(HTMLInputElement, "accept");
    impl.Element.reflectStringAttribute(HTMLInputElement, "alt");
    impl.Element.reflectStringAttribute(HTMLInputElement, "max");
    impl.Element.reflectStringAttribute(HTMLInputElement, "min");
    impl.Element.reflectStringAttribute(HTMLInputElement, "pattern");
    impl.Element.reflectStringAttribute(HTMLInputElement, "placeholder");
    impl.Element.reflectStringAttribute(HTMLInputElement, "step");
    impl.Element.reflectStringAttribute(HTMLInputElement,
                                        "dirname", "dirName");
    impl.Element.reflectStringAttribute(HTMLInputElement,
                                        "value", "defaultValue");

    impl.Element.reflectBooleanAttribute(HTMLInputElement, "multiple");
    impl.Element.reflectBooleanAttribute(HTMLInputElement, "required");
    impl.Element.reflectBooleanAttribute(HTMLInputElement,
                                         "readonly", "readOnly");
    impl.Element.reflectBooleanAttribute(HTMLInputElement,
                                        "checked", "defaultChecked");

    impl.Element.reflectIntegerAttribute(HTMLInputElement, "size", 20, null,
                                         1, null, 1);
    impl.Element.reflectIntegerAttribute(HTMLInputElement, "maxlength", -1,
                                         "maxLength", 0, null, 0);

    // impl.Element.reflectURLAttribute(HTMLInputElement, "src");

    impl.Element.reflectEnumeratedAttribute(HTMLInputElement, "autocomplete",
                                            null,
                                            {
                                                on: "on",
                                                off: "off"
                                            });

    impl.Element.reflectEnumeratedAttribute(HTMLInputElement, "type", null,
                                            {
                                                hidden: "hidden",
                                                text: "text",
                                                search: "search",
                                                tel: "tel",
                                                url: "url",
                                                email: "email",
                                                password: "password",
                                                datetime: "datetime",
                                                date: "date",
                                                month: "month",
                                                week: "week",
                                                time: "time",
                                                "datetime-local": "datetime-local",
                                                number: "number",
                                                range: "range",
                                                color: "color",
                                                checkbox: "checkbox",
                                                radio: "radio",
                                                file: "file",
                                                submit: "submit",
                                                image: "image",
                                                reset: "reset",
                                                button: "button",
                                            }, "text");


    impl.Element.reflectStringAttribute(HTMLInputElement,
                                        "formtarget", "formTarget");
    impl.Element.reflectBooleanAttribute(HTMLInputElement,
                                         "formnovalidate", "formNoValidate");
    impl.Element.reflectEnumeratedAttribute(HTMLInputElement,
                                            "formmethod", "formMethod", {
                                                get: "get",
                                                post: "post"
                                            }, "get");
    impl.Element.reflectEnumeratedAttribute(HTMLInputElement,
                                            "formenctype", "formEnctype", {
        "application/x-www-form-urlencoded":"application/x-www-form-urlencoded",
        "multipart/form-data":"multipart/form-data",
        "text/plain": "text/plain"
    }, "application/x-www-form-urlencoded");

    return HTMLInputElement;
});

defineLazyProperty(impl, "HTMLKeygenElement", function() {
    function HTMLKeygenElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLKeygenElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLKeygenElement"),
    });

    impl.Element.reflectStringAttribute(HTMLKeygenElement, "name");
    impl.Element.reflectBooleanAttribute(HTMLKeygenElement, "disabled");
    impl.Element.reflectBooleanAttribute(HTMLKeygenElement, "autofocus");

    impl.Element.reflectStringAttribute(HTMLKeygenElement, "challenge");
    impl.Element.reflectEnumeratedAttribute(HTMLKeygenElement, "keytype", null,
                                            { rsa: "rsa" }, "rsa");

    return HTMLKeygenElement;
});

defineLazyProperty(impl, "HTMLLIElement", function() {
    function HTMLLIElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLLIElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLLIElement"),
    });

    impl.Element.reflectIntegerAttribute(HTMLLIElement, "value", 0);

    return HTMLLIElement;
});

defineLazyProperty(impl, "HTMLLabelElement", function() {
    function HTMLLabelElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLLabelElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLLabelElement"),
    });

    impl.Element.reflectStringAttribute(HTMLLabelElement, "for", "htmlFor");

    return HTMLLabelElement;
});

defineLazyProperty(impl, "HTMLLegendElement", function() {
    function HTMLLegendElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLLegendElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLLegendElement"),
    });

    return HTMLLegendElement;
});

defineLazyProperty(impl, "HTMLLinkElement", function() {
    function HTMLLinkElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLLinkElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLLinkElement"),
    });

    // XXX: still have to reflect URL attribute href
    // and DOMSettableTokenList sizes also DOMTokenList relList
    impl.Element.reflectStringAttribute(HTMLLinkElement, "rel");
    impl.Element.reflectStringAttribute(HTMLLinkElement, "media");
    impl.Element.reflectStringAttribute(HTMLLinkElement, "hreflang");
    impl.Element.reflectStringAttribute(HTMLLinkElement, "type");



    return HTMLLinkElement;
});

defineLazyProperty(impl, "HTMLMapElement", function() {
    function HTMLMapElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLMapElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLMapElement"),
    });

    impl.Element.reflectStringAttribute(HTMLMapElement, "name");

    return HTMLMapElement;
});

defineLazyProperty(impl, "HTMLMenuElement", function() {
    function HTMLMenuElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLMenuElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLMenuElement"),
    });

    impl.Element.reflectStringAttribute(HTMLMenuElement, "type");
    impl.Element.reflectStringAttribute(HTMLMenuElement, "label");

    return HTMLMenuElement;
});

defineLazyProperty(impl, "HTMLMetaElement", function() {
    function HTMLMetaElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLMetaElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLMetaElement"),
    });

    impl.Element.reflectStringAttribute(HTMLMetaElement, "name");
    impl.Element.reflectStringAttribute(HTMLMetaElement, "content");
    impl.Element.reflectStringAttribute(HTMLMetaElement,
                                        "http-equiv", "httpEquiv");

    return HTMLMetaElement;
});

defineLazyProperty(impl, "HTMLMeterElement", function() {
    function HTMLMeterElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLMeterElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLMeterElement"),
    });

    return HTMLMeterElement;
});

defineLazyProperty(impl, "HTMLModElement", function() {
    function HTMLModElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLModElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLModElement"),
    });

    impl.Element.reflectStringAttribute(HTMLModElement, "cite");
    impl.Element.reflectStringAttribute(HTMLModElement, "datetime", "dateTime");

    return HTMLModElement;
});

defineLazyProperty(impl, "HTMLOListElement", function() {
    function HTMLOListElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLOListElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLOListElement"),

        // Utility function (see the start attribute default value). Returns
        // the number of <li> children of this element
        _numitems: attribute(function() {
            var items = 0;
            this.childNodes.forEach(function(n) {
                if (n.nodeType === ELEMENT_NODE && n.tagName === "LI")
                    items++;
            });
            return items;
        }),
    });

    impl.Element.reflectStringAttribute(HTMLOListElement, "type");
    impl.Element.reflectBooleanAttribute(HTMLOListElement, "reversed");
    impl.Element.reflectIntegerAttribute(HTMLOListElement, "start",
                                         function() {
                                             // The default value of the
                                             // start attribute is 1 unless
                                             // the list is reversed. Then it
                                             // is the # of li children
                                             if (this.reversed)
                                                 return this._numitems;
                                             else
                                                 return 1;
                                         });


    return HTMLOListElement;
});

defineLazyProperty(impl, "HTMLObjectElement", function() {
    function HTMLObjectElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLObjectElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLObjectElement"),
    });

    // impl.Element.reflectURLAttribute(HTMLObjectElement, "data");
    impl.Element.reflectStringAttribute(HTMLObjectElement, "type");
    impl.Element.reflectStringAttribute(HTMLObjectElement, "name");
    impl.Element.reflectStringAttribute(HTMLObjectElement, "usemap", "useMap");
    impl.Element.reflectBooleanAttribute(HTMLObjectElement,
                                         "typemustmatch", "typeMustMatch");
    impl.Element.reflectStringAttribute(HTMLObjectElement, "width");
    impl.Element.reflectStringAttribute(HTMLObjectElement, "height");

    return HTMLObjectElement;
});

defineLazyProperty(impl, "HTMLOptGroupElement", function() {
    function HTMLOptGroupElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLOptGroupElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLOptGroupElement"),
    });

    impl.Element.reflectBooleanAttribute(HTMLOptGroupElement, "disabled");
    impl.Element.reflectStringAttribute(HTMLOptGroupElement, "label");

    return HTMLOptGroupElement;
});

defineLazyProperty(impl, "HTMLOptionElement", function() {
    function HTMLOptionElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLOptionElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLOptionElement"),
    });

    impl.Element.reflectBooleanAttribute(HTMLOptionElement, "disabled");
    impl.Element.reflectBooleanAttribute(HTMLOptionElement,
                                         "selected", "defaultSelected");
    impl.Element.reflectStringAttribute(HTMLOptionElement, "label");

    return HTMLOptionElement;
});

defineLazyProperty(impl, "HTMLOutputElement", function() {
    function HTMLOutputElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLOutputElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLOutputElement"),
    });

    // XXX Reflect for/htmlFor as a settable token list
    impl.Element.reflectStringAttribute(HTMLOutputElement, "name");

    return HTMLOutputElement;
});

defineLazyProperty(impl, "HTMLParagraphElement", function() {
    function HTMLParagraphElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLParagraphElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLParagraphElement"),
    });

    return HTMLParagraphElement;
});

defineLazyProperty(impl, "HTMLParamElement", function() {
    function HTMLParamElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLParamElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLParamElement"),
    });

    return HTMLParamElement;
});

defineLazyProperty(impl, "HTMLPreElement", function() {
    function HTMLPreElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLPreElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLPreElement"),
    });

    return HTMLPreElement;
});

defineLazyProperty(impl, "HTMLProgressElement", function() {
    function HTMLProgressElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLProgressElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLProgressElement"),
    });

    impl.Element.reflectPositiveFloatAttribute(HTMLProgressElement, "max", 1.0);

    return HTMLProgressElement;
});

defineLazyProperty(impl, "HTMLQuoteElement", function() {
    function HTMLQuoteElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLQuoteElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLQuoteElement"),
    });

    impl.Element.reflectStringAttribute(HTMLQuoteElement, "cite");

    return HTMLQuoteElement;
});

// HTMLScriptElement used to be here, but now has its own file.

defineLazyProperty(impl, "HTMLSelectElement", function() {
    function HTMLSelectElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLSelectElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLSelectElement"),
    });

    impl.Element.reflectStringAttribute(HTMLSelectElement, "name");
    impl.Element.reflectBooleanAttribute(HTMLSelectElement, "disabled");
    impl.Element.reflectBooleanAttribute(HTMLSelectElement, "autofocus");

    impl.Element.reflectBooleanAttribute(HTMLSelectElement, "multiple");
    impl.Element.reflectBooleanAttribute(HTMLSelectElement, "required");
    impl.Element.reflectIntegerAttribute(HTMLSelectElement, "size", 0);

    return HTMLSelectElement;
});

defineLazyProperty(impl, "HTMLSourceElement", function() {
    function HTMLSourceElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLSourceElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLSourceElement"),
    });

    // impl.Element.reflectURLAttribute(HTMLSourceElement, "src");
    impl.Element.reflectStringAttribute(HTMLSourceElement, "type");
    impl.Element.reflectStringAttribute(HTMLSourceElement, "media");

    return HTMLSourceElement;
});

defineLazyProperty(impl, "HTMLSpanElement", function() {
    function HTMLSpanElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLSpanElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLSpanElement"),
    });

    return HTMLSpanElement;
});

defineLazyProperty(impl, "HTMLStyleElement", function() {
    function HTMLStyleElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLStyleElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLStyleElement"),
    });

    impl.Element.reflectStringAttribute(HTMLStyleElement, "media");
    impl.Element.reflectStringAttribute(HTMLStyleElement, "type");
    impl.Element.reflectBooleanAttribute(HTMLStyleElement, "scoped");

    return HTMLStyleElement;
});

defineLazyProperty(impl, "HTMLTableCaptionElement", function() {
    function HTMLTableCaptionElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTableCaptionElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTableCaptionElement"),
    });

    return HTMLTableCaptionElement;
});

defineLazyProperty(impl, "HTMLTableCellElement", function() {
    function HTMLTableCellElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTableCellElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTableCellElement"),
    });

    impl.Element.reflectIntegerAttribute(HTMLTableCellElement, "colspan", 1,
                                         "colSpan", 1, null, 1);
    impl.Element.reflectIntegerAttribute(HTMLTableCellElement, "rowspan", 1,
                                         "rowSpan");
    //XXX Also reflect settable token list headers


    return HTMLTableCellElement;
});

defineLazyProperty(impl, "HTMLTableColElement", function() {
    function HTMLTableColElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTableColElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTableColElement"),
    });

    impl.Element.reflectIntegerAttribute(HTMLTableColElement, "span", 1, null,
                                         1, null, 1);


    return HTMLTableColElement;
});

defineLazyProperty(impl, "HTMLTableElement", function() {
    function HTMLTableElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTableElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTableElement"),
    });

    impl.Element.reflectStringAttribute(HTMLTableElement, "border");

    return HTMLTableElement;
});

defineLazyProperty(impl, "HTMLTableRowElement", function() {
    function HTMLTableRowElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTableRowElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTableRowElement"),
    });

    return HTMLTableRowElement;
});

defineLazyProperty(impl, "HTMLTableSectionElement", function() {
    function HTMLTableSectionElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTableSectionElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTableSectionElement"),
    });

    return HTMLTableSectionElement;
});

defineLazyProperty(impl, "HTMLTextAreaElement", function() {
    function HTMLTextAreaElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTextAreaElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTextAreaElement"),
    });


    impl.Element.reflectStringAttribute(HTMLTextAreaElement, "name");
    impl.Element.reflectBooleanAttribute(HTMLTextAreaElement, "disabled");
    impl.Element.reflectBooleanAttribute(HTMLTextAreaElement, "autofocus");

    impl.Element.reflectStringAttribute(HTMLTextAreaElement, "placeholder");
    impl.Element.reflectStringAttribute(HTMLTextAreaElement, "wrap");
    impl.Element.reflectStringAttribute(HTMLTextAreaElement,
                                        "dirname", "dirName");

    impl.Element.reflectBooleanAttribute(HTMLTextAreaElement, "required");
    impl.Element.reflectBooleanAttribute(HTMLTextAreaElement,
                                         "readonly", "readOnly");

    impl.Element.reflectIntegerAttribute(HTMLTextAreaElement, "rows", 2, null,
                                         1, null, 1);
    impl.Element.reflectIntegerAttribute(HTMLTextAreaElement, "cols", 20, null,
                                         1, null, 1);
    impl.Element.reflectIntegerAttribute(HTMLTextAreaElement,
                                         "maxlength", -1, "maxLength",
                                         0, null, 0);


    return HTMLTextAreaElement;
});

defineLazyProperty(impl, "HTMLTimeElement", function() {
    function HTMLTimeElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTimeElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTimeElement"),
    });

    impl.Element.reflectStringAttribute(HTMLTimeElement, "datetime","dateTime");
    impl.Element.reflectBooleanAttribute(HTMLTimeElement, "pubdate", "pubDate");

    return HTMLTimeElement;
});

defineLazyProperty(impl, "HTMLTitleElement", function() {
    function HTMLTitleElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTitleElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTitleElement"),
    });

    return HTMLTitleElement;
});

defineLazyProperty(impl, "HTMLTrackElement", function() {
    function HTMLTrackElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLTrackElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLTrackElement"),
    });

    // impl.Element.reflectURLAttribute(HTMLTrackElement, "src");
    impl.Element.reflectStringAttribute(HTMLTrackElement, "srclang");
    impl.Element.reflectStringAttribute(HTMLTrackElement, "label");
    impl.Element.reflectBooleanAttribute(HTMLTrackElement, "default");
    impl.Element.reflectEnumeratedAttribute(HTMLTrackElement, "kind", null,
                                            {
                                                subtitles: "subtitles",
                                                captions: "captions",
                                                descriptions: "descriptions",
                                                chapters: "chapters",
                                                metadata: "metadata"
                                            },
                                            "subtitles");


    return HTMLTrackElement;
});

defineLazyProperty(impl, "HTMLUListElement", function() {
    function HTMLUListElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLUListElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLUListElement"),
    });

    return HTMLUListElement;
});

defineLazyProperty(impl, "HTMLUnknownElement", function() {
    function HTMLUnknownElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLUnknownElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLUnknownElement"),
    });

    return HTMLUnknownElement;
});

defineLazyProperty(impl, "HTMLMediaElement", function() {
    function HTMLMediaElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLMediaElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLMediaElement"),
    });

    // impl.Element.reflectURLAttribute(HTMLMediaElement, "src");
    impl.Element.reflectStringAttribute(HTMLMediaElement,
                                        "crossorigin", "crossOrigin");
    impl.Element.reflectEnumeratedAttribute(HTMLMediaElement, "preload", null,
                                            {
                                                none: "none",
                                                metadata: "metadata",
                                                auto: "auto",
                                                "": "auto"
                                            },
                                            "metadata" // user-agent defined
                                           );

    impl.Element.reflectBooleanAttribute(HTMLMediaElement, "loop");
    impl.Element.reflectBooleanAttribute(HTMLMediaElement, "autoplay");
    impl.Element.reflectStringAttribute(HTMLMediaElement,
                                        "mediagroup", "mediaGroup");
    impl.Element.reflectBooleanAttribute(HTMLMediaElement, "controls");
    impl.Element.reflectBooleanAttribute(HTMLMediaElement,
                                         "muted", "defaultMuted");

    return HTMLMediaElement;
});

defineLazyProperty(impl, "HTMLAudioElement", function() {
    function HTMLAudioElement(doc, localName, prefix) {
        impl.HTMLMediaElement.call(this, doc, localName, prefix);
    }

    HTMLAudioElement.prototype = O.create(impl.HTMLMediaElement.prototype, {
        _idlName: constant("HTMLAudioElement"),
    });

    return HTMLAudioElement;
});

defineLazyProperty(impl, "HTMLVideoElement", function() {
    function HTMLVideoElement(doc, localName, prefix) {
        impl.HTMLMediaElement.call(this, doc, localName, prefix);
    }

    HTMLVideoElement.prototype = O.create(impl.HTMLMediaElement.prototype, {
        _idlName: constant("HTMLVideoElement"),
    });

    // impl.Element.reflectURLAttribute(HTMLVideoElement,"poster");
    impl.Element.reflectIntegerAttribute(HTMLVideoElement, "width", 0, null,0);
    impl.Element.reflectIntegerAttribute(HTMLVideoElement, "height", 0, null,0);

    return HTMLVideoElement;
});

defineLazyProperty(impl, "HTMLTableDataCellElement", function() {
    function HTMLTableDataCellElement(doc, localName, prefix) {
        impl.HTMLTableCellElement.call(this, doc, localName, prefix);
    }

    HTMLTableDataCellElement.prototype = O.create(impl.HTMLTableCellElement.prototype, {
        _idlName: constant("HTMLTableDataCellElement"),
    });

    return HTMLTableDataCellElement;
});

defineLazyProperty(impl, "HTMLTableHeaderCellElement", function() {
    function HTMLTableHeaderCellElement(doc, localName, prefix) {
        impl.HTMLTableCellElement.call(this, doc, localName, prefix);
    }

    HTMLTableHeaderCellElement.prototype = O.create(impl.HTMLTableCellElement.prototype, {
        _idlName: constant("HTMLTableHeaderCellElement"),
    });

    impl.Element.reflectEnumeratedAttribute(HTMLTableHeaderCellElement,
                                            "scope", null, {
                                                row: "row",
                                                col: "col",
                                                rowgroup: "rowgroup",
                                                colgroup: "colgroup",
                                            },
                                            "");

    return HTMLTableHeaderCellElement;
});

defineLazyProperty(impl, "HTMLFrameSetElement", function() {
    function HTMLFrameSetElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLFrameSetElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLFrameSetElement"),
    });

    return HTMLFrameSetElement;
});

defineLazyProperty(impl, "HTMLFrameElement", function() {
    function HTMLFrameElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
    }

    HTMLFrameElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLFrameElement"),
    });

    return HTMLFrameElement;
});



/************************************************************************
 *  src/impl/HTMLScriptElement.js
 ************************************************************************/

//@line 1 "src/impl/HTMLScriptElement.js"
defineLazyProperty(impl, "HTMLScriptElement", function() {
    const JavaScriptMimeTypes = {
        "application/ecmascript":true,
        "application/javascript":true,
        "application/x-ecmascript":true,
        "application/x-javascript":true,
        "text/ecmascript":true,
        "text/javascript":true,
        "text/javascript1.0":true,
        "text/javascript1.1":true,
        "text/javascript1.2":true,
        "text/javascript1.3":true,
        "text/javascript1.4":true,
        "text/javascript1.5":true,
        "text/jscript":true,
        "text/livescript":true,
        "text/x-ecmascript":true,
        "text/x-javascript":true
    };


    function HTMLScriptElement(doc, localName, prefix) {
        impl.HTMLElement.call(this, doc, localName, prefix);
        // Internal script flags, used by the parser and elsewhere
        this._already_started = false;
        this._parser_inserted = false;
        this._ready_to_execute = false;
        this._force_async = true;
        this._had_async_content_attribute = false;
        this._creatorDocument = doc; // in case ownerDocument changes later
    }

    // Script elements that are not parser inserted must call _prepare() when:
    // 1) a script is inserted into the document.  (see _roothook below)
    // 2) the script's children change
    //   XXX: need to make this one happen
    //   I could use a proxy array for childNodes and handle that here
    //   That might be more efficient than adding hooks in Node.
    //   Also, I sent email to whatwg mailing list about this.
    //   Firefox actually triggers a script if a text node child
    //   changes from the empty string to non-empty, and that would
    //   be hard to have a hook for. Or, I could use the modtime thing
    //   to look for any changes on any descendant and then check the
    //   text property. The transition from "" to non-empty text would
    //   be a _prepare() trigger.  But I'm hoping that the spec will change
    //   so that any child insertion (including an empty text node) \
    //   is enough.
    //
    // 3) when the a src attribute is defined
    //   (See _newattrhook below);
    //


    HTMLScriptElement.prototype = O.create(impl.HTMLElement.prototype, {
        _idlName: constant("HTMLScriptElement"),

        // Script elements need to know when they're inserted into the
        // document, so they define this hook method
        _roothook: constant(function() {
            if (!this._parser_inserted) this._prepare();
        }),

        // The Script element needs to know when its src and async attrs are set
        _newattrhook: constant(function(name, value) {
            switch(name) {
            case 'async':
                this._force_async = false;
                break;
            case 'src':
                if (!this._parser_inserted && this.rooted) this._prepare();
                break;
            }
        }),

        // The Script element needs to know when a child is added
        // This hook is only for direct children: it does not bubble up
        _addchildhook: constant(function(child) {
            // XXX what if multiple children are added at once
            // via a DocumentFragment, do we run all of them or only the first?
            if (!this._parser_inserted && this.rooted) this._prepare();
        }),

        // Finally, it needs to know when a Text child has changed
        // This hook only gets triggered for direct children, not all
        // descendants
        _textchangehook: constant(function(child) {
            if (!this._parser_inserted && this.rooted) this._prepare();
        }),

        // The async idl attr doesn't quite reflect the async content attr
        async: attribute(
            function() {
                if (this._force_async) return true;
                return this.getAttribute("async");
            },
            function(value) {
                this._force_async = false;
                if (value) {
                    this._setattr("async", "");
                }
                else {
                    this.removeAttribute("async");
                }
            }
        ),

        text: attribute(
            function() {
                var s = "";
                for(var i = 0, n = this.childNodes.length; i < n; i++) {
                    var child = this.childNodes[i];
                    if (child.nodeType === TEXT_NODE)
                        s += child._data;
                }
                return s;
            },
            function(value) {
                this.removeChildren();
                if (value !== null && value !== "") {
                    this.appendChild(this.ownerDocument.createTextNode(value));
                }
            }
        ),

        // The HTML "Prepare a Script" algorithm
        _prepare: constant(function() {
            // If the script element is marked as having "already started",
            // then the user agent must abort these steps at this point. The
            // script is not executed.
            if (this._already_started) return;

            // If the element has its "parser-inserted" flag set, then set
            // was-parser-inserted to true and unset the element's
            // "parser-inserted" flag. Otherwise, set was-parser-inserted to
            // false.
            var was_parser_inserted = this._parser_inserted;
            this._parser_inserted = false;

            // If was-parser-inserted is true and the element does not have an
            // async attribute, then set the element's "force-async" flag to
            // true.
            if (was_parser_inserted && !this.hasAttribute("async"))
                this._force_async = true;

            // If the element has no src attribute, and its child nodes, if
            // any, consist only of comment nodes and empty text nodes, then
            // the user agent must abort these steps at this point. The script
            // is not executed.
            if (!this.hasAttribute("src") && this.text === "") return;

            // If the element is not in a Document, then the user agent must
            // abort these steps at this point. The script is not executed.
            if (!this.rooted) return;

            // If either:
            //     the script element has a type attribute and its value is the
            //     empty string, or the script element has no type attribute
            //     but it has a language attribute and that attribute's value
            //     is the empty string, or the script element has neither a
            //     type attribute nor a language attribute, then
            //
            // ...let the script block's type for this script element be
            // "text/javascript".
            //
            // Otherwise, if the script element has a type attribute, let the
            // script block's type for this script element be the value of that
            // attribute with any leading or trailing sequences of space
            // characters removed.
            //
            // Otherwise, the element has a non-empty language attribute; let
            // the script block's type for this script element be the
            // concatenation of the string "text/" followed by the value of the
            // language attribute.
            //
            // The language attribute is never conforming, and is always
            // ignored if there is a type attribute present.
            var hastype = this.hasAttribute("type");
            var typeattr = hastype ? this.getAttribute("type") : undefined;
            var haslang = this.hasAttribute("language");
            var langattr = haslang ? this.getAttribute("language") : undefined;
            var scripttype;

            if ((typeattr === "") ||
                (!hastype && langattr === "") ||
                (!hastype && !haslang)) {
                scripttype = "text/javascript";
            }
            else if (hastype) {
                // Can't use trim() here, because it has a different
                // definition of whitespace than html does
                scripttype = htmlTrim(typeattr);
            }
            else {
                scripttype = "text/" + langattr;
            }

            // If the user agent does not support the scripting language given
            // by the script block's type for this script element, then the
            // user agent must abort these steps at this point. The script is
            // not executed.
            if (!JavaScriptMimeTypes[toLowerCase(scripttype)]) return;

            // If was-parser-inserted is true, then flag the element as
            // "parser-inserted" again, and set the element's "force-async"
            // flag to false.
            if (was_parser_inserted) {
                this._parser_inserted = true;
                this._force_async = false;
            }

            // The user agent must set the element's "already started" flag.
            this._already_started = true;

            // The state of the element at this moment is later used to
            // determine the script source.
            this._script_text = this.text;  // We'll use this in _execute

            // If the element is flagged as "parser-inserted", but the
            // element's Document is not the Document of the parser that
            // created the element, then abort these steps.
            if (this._parser_inserted &&
                this.ownerDocument !== this._creatorDocument)
                return;  // Script was moved to a new document

            // If scripting is disabled for the script element, then the user
            // agent must abort these steps at this point. The script is not
            // executed.
            //
            // The definition of scripting is disabled means that, amongst
            // others, the following scripts will not execute: scripts in
            // XMLHttpRequest's responseXML documents, scripts in
            // DOMParser-created documents, scripts in documents created by
            // XSLTProcessor's transformToDocument feature, and scripts that
            // are first inserted by a script into a Document that was created
            // using the createDocument() API. [XHR] [DOMPARSING] [DOMCORE]
            //
            // XXX: documents with a browsing context have scripting on
            // (except iframes with the sandbox attr). Standalone docs do not.
            // Its not clear to me when I should set this flag.  dom.js is
            // in a weird situation since we don't really have a window yet,
            // but we do want to run scripts.  For now, I think I'll have
            // the parser set this, and also set it on the initial
            // global document.
            //
            if (!this.ownerDocument._scripting_enabled) return;

            // If the script element has an event attribute and a for
            // attribute, then run these substeps:
            //
            //     Let for be the value of the for attribute.
            //
            //     Let event be the value of the event attribute.
            //
            //     Strip leading and trailing whitespace from event and for.
            //
            //     If for is not an ASCII case-insensitive match for the string
            //     "window", then the user agent must abort these steps at this
            //     point. The script is not executed.
            //
            //     If event is not an ASCII case-insensitive match for either
            //     the string "onload" or the string "onload()", then the user
            //     agent must abort these steps at this point. The script is
            //     not executed.

            var forattr = this.getAttribute("for") || "";
            var eventattr = this.getAttribute("event") || "";
            if (forattr || eventattr) {
                forattr = toLowerCase(htmlTrim(forattr));
                eventattr = toLowerCase(htmlTrim(eventattr));
                if (forattr !== "window" ||
                    (event !== "onload" && event !== "onload()"))
                    return;
            }


            // If the script element has a charset attribute, then let the
            // script block's character encoding for this script element be the
            // encoding given by the charset attribute.
            //
            // Otherwise, let the script block's fallback character encoding
            // for this script element be the same as the encoding of the
            // document itself.
            //
            // Only one of these two pieces of state is set.
            if (this.hasAttribute("charset")) {
                // XXX: ignoring charset issues for now
            }
            else {
                // XXX: ignoring charset issues for now
            }

            // If the element has a src attribute whose value is not the empty
            // string, then the value of that attribute must be resolved
            // relative to the element, and if that is successful, the
            // specified resource must then be fetched, from the origin of the
            // element's Document.
            //
            // If the src attribute's value is the empty string or if it could
            // not be resolved, then the user agent must queue a task to fire a
            // simple event named error at the element, and abort these steps.
            //
            // For historical reasons, if the URL is a javascript: URL, then
            // the user agent must not, despite the requirements in the
            // definition of the fetching algorithm, actually execute the
            // script in the URL; instead the user agent must act as if it had
            // received an empty HTTP 400 response.
            //
            // For performance reasons, user agents may start fetching the
            // script as soon as the attribute is set, instead, in the hope
            // that the element will be inserted into the document. Either way,
            // once the element is inserted into the document, the load must
            // have started. If the UA performs such prefetching, but the
            // element is never inserted in the document, or the src attribute
            // is dynamically changed, then the user agent will not execute the
            // script, and the fetching process will have been effectively
            // wasted.
            if (this.hasAttribute("src")) {
                // XXX
                // The spec for handling this is really, really complicated.
                // For now, I'm just going to try to get something basic working

                var url = this.getAttribute("src");

                if (this.ownerDocument._parser) {
                    this.ownerDocument._parser.pause();
                }

/*
                // XXX: this is a hack
                // If we're running in node, and the document has an
                // _address, then we can resolve the URL
                if (this.ownerDocument._address &&
                    typeof require === "function") {
                    url = require('url').resolve(this.ownerDocument._address,
                                                 url);
                }
*/
                // Resolve the script url against the document url
                var documenturl = new URL(this.ownerDocument.defaultView.location.href);
                url = documenturl.resolve(url);


                // XXX: this is experimental
                // If we're in a web worker, use importScripts
                // to load and execute the script.
                // Maybe this will give us better error messages
                if (global.importScripts) {
                    try {
                        importScripts(url);
                    }
                    catch(e) {
                        error(e + " " + e.stack);
                    }
                    finally {
                        this.ownerDocument._parser.resume();
                    }
                }
                else {

                    var script = this;
                    var xhr = new XMLHttpRequest();

                    // Web workers support this handler but not the old
                    // onreadystatechange handler
                    xhr.onloadend = function() {
                        if (xhr.status === 200 ||
                            xhr.status === 0 /* file:// urls */) {
                            script._script_text = xhr.responseText;
                            script._execute();
                            delete script._script_text;
                        }
                        // Do this even if we failed
                        if (script.ownerDocument._parser) {
                            script.ownerDocument._parser.resume();
                        }
                    };

                    // My node version of XHR responds to this handler but
                    // not to onloadend above.
                    xhr.onreadystatechange = function() {
                        if (xhr.readyState !== 4) return;
                        if (xhr.status === 200 ||
                            xhr.status === 0 /* file:// urls */) {
                            script._script_text = xhr.responseText;
                            script._execute();
                            delete script._script_text;
                        }

                        // Do this even if we failed
                        if (script.ownerDocument._parser) {
                            script.ownerDocument._parser.resume();
                        }
                    }

                    xhr.open("GET", url);
                    xhr.send();
                }
            }
            else {

                // XXX
                // Just execute inlines scripts now.
                // Later, I've got to deal with the all the cases below

                this._execute();
            }

            // Then, the first of the following options that describes the
            // situation must be followed:

            // If the element has a src attribute, and the element has a defer
            // attribute, and the element has been flagged as
            // "parser-inserted", and the element does not have an async
            // attribute

            //     The element must be added to the end of the list of scripts
            //     that will execute when the document has finished parsing
            //     associated with the Document of the parser that created the
            //     element.

            //     The task that the networking task source places on the task
            // queue once the fetching algorithm has completed must set the
            // element's "ready to be parser-executed" flag. The parser will
            // handle executing the script.  If the element has a src
            // attribute, and the element has been flagged as
            // "parser-inserted", and the element does not have an async
            // attribute

            //     The element is the pending parsing-blocking script of the
            //     Document of the parser that created the element. (There can
            //     only be one such script per Document at a time.)

            //     The task that the networking task source places on the task
            // queue once the fetching algorithm has completed must set the
            // element's "ready to be parser-executed" flag. The parser will
            // handle executing the script.  If the element does not have a src
            // attribute, and the element has been flagged as
            // "parser-inserted", and the Document of the HTML parser or XML
            // parser that created the script element has a style sheet that is
            // blocking scripts

            //     The element is the pending parsing-blocking script of the
            //     Document of the parser that created the element. (There can
            //     only be one such script per Document at a time.)

            //     Set the element's "ready to be parser-executed" flag. The
            // parser will handle executing the script.  If the element has a
            // src attribute, does not have an async attribute, and does not
            // have the "force-async" flag set

            //     The element must be added to the end of the list of scripts
            //     that will execute in order as soon as possible associated
            //     with the Document of the script element at the time the
            //     prepare a script algorithm started.

            //     The task that the networking task source places on the task
            //     queue once the fetching algorithm has completed must run the
            //     following steps:

            //         If the element is not now the first element in the list
            //         of scripts that will execute in order as soon as
            //         possible to which it was added above, then mark the
            //         element as ready but abort these steps without executing
            //         the script yet.

            //         Execution: Execute the script block corresponding to the
            //         first script element in this list of scripts that will
            //         execute in order as soon as possible.

            //         Remove the first element from this list of scripts that
            //         will execute in order as soon as possible.

            //         If this list of scripts that will execute in order as
            //         soon as possible is still not empty and the first entry
            //         has already been marked as ready, then jump back to the
            //         step labeled execution.

            // If the element has a src attribute

            //     The element must be added to the set of scripts that will
            //     execute as soon as possible of the Document of the script
            //     element at the time the prepare a script algorithm started.

            //     The task that the networking task source places on
            //     the task queue once the fetching algorithm has
            //     completed must execute the script block and then
            //     remove the element from the set of scripts that
            //     will execute as soon as possible.
            //
            // Otherwise The user agent must immediately execute the
            // script block, even if other scripts are already
            // executing.
        }),

        _execute: constant(function() {
            // We test this in _prepare(), but the spec says we
            // have to check again here.
            if (this._parser_inserted &&
                this.ownerDocument !== this._creatorDocument) return;

            // XXX
            // For now, we're just doing inline scripts, so I'm skipping
            // the steps about if the load was not successful.
            var code = this._script_text;

            // If the script is from an external file, then increment the
            // ignore-destructive-writes counter of the script element's
            // Document. Let neutralized doc be that Document.
            // XXX: ignoring this for inline scripts for now.

            // XXX
            // There is actually more to executing a script than this.
            // See http://www.whatwg.org/specs/web-apps/current-work/multipage/webappapis.html#create-a-script
            try {
// XXX For now, we're just assuming that there is never more than
// one document at a time, and all scripts get executed against the
// same global object.
//                var olddoc = global.document;
//                global.document = wrap(this.ownerDocument);
                evalScript(code);
//                global.document = olddoc;
            }
            catch(e) {
                // XXX fire an onerror event before reporting
                error(e + " " + e.stack);
            }

            // Decrement the ignore-destructive-writes counter of neutralized
            // doc, if it was incremented in the earlier step.

            // If the script is from an external file, fire a simple event
            // named load at the script element.

            // Otherwise, the script is internal; queue a task to fire a simple
            // event named load at the script element.

        }),
    });

    // XXX impl.Element.reflectURLAttribute(HTMLScriptElement, "src");
    impl.Element.reflectStringAttribute(HTMLScriptElement, "type");
    impl.Element.reflectStringAttribute(HTMLScriptElement, "charset");
    impl.Element.reflectBooleanAttribute(HTMLScriptElement, "defer");


    return HTMLScriptElement;
});



/************************************************************************
 *  src/impl/HTMLParser.js
 ************************************************************************/

//@line 1 "src/impl/HTMLParser.js"
/*
 * This file contains an implementation of the HTML parsing algorithm.
 * The algorithm and the implementation are complex because HTML
 * explicitly defines how the parser should behave for all possible
 * valid and invalid inputs.
 *
 * Usage:
 *
 * The file defines a single HTMLParser() function, which dom.js exposes
 * publicly as document.implementation.mozHTMLParser(). This is a
 * factory function, not a constructor.
 *
 * When you call document.implementation.mozHTMLParser(), it returns
 * an object that has parse() and document() methods. To parse HTML text,
 * pass the text (in one or more chunks) to the parse() method.  When
 * you've passed all the text (on the last chunk, or afterward) pass
 * true as the second argument to parse() to tell the parser that there
 * is no more coming. Call document() to get the document object that
 * the parser is parsing into.  You can call this at any time, before
 * or after calling parse().
 *
 * The first argument to mozHTMLParser is the absolute URL of the document.
 *
 * The second argument is optional and is for internal use only.  Pass an
 * element as the fragmentContext to do innerHTML parsing for the
 * element.  To do innerHTML parsing on a document, pass null. Otherwise,
 * omit the 2nd argument. See HTMLElement.innerHTML for an example.  Note
 * that if you pass a context element, the end() method will return an
 * unwrapped document instead of a wrapped one.
 *
 * Implementation details:
 *
 * This is a long file of almost 7000 lines. It is structured as one
 * big function nested within another big function.  The outer
 * function defines a bunch of constant data, utility functions
 * that use that data, and a couple of classes used by the parser.
 * The outer function also defines and returns the
 * inner function. This inner function is the HTMLParser factory
 * function that implements the parser and holds all the parser state
 * as local variables.  The HTMLParser function is quite big because
 * it defines many nested functions that use those local variables.
 *
 * There are three tightly coupled parser stages: a scanner, a
 * tokenizer and a tree builder. In a (possibly misguided) attempt at
 * efficiency, the stages are not implemented as separate classes:
 * everything shares state and is (mostly) implemented in imperative
 * (rather than OO) style.
 *
 * The stages of the parser work like this: When the client code calls
 * the parser's parse() method, the specified string is passed to
 * scanChars(). The scanner loops through that string and passes characters
 * (sometimes one at a time, sometimes in chunks) to the tokenizer stage.
 * The tokenizer groups the characters into tokens: tags, endtags, runs
 * of text, comments, doctype declarations, and the end-of-file (EOF)
 * token.  These tokens are then passed to the tree building stage via
 * the insertToken() function.  The tree building stage builds up the
 * document tree.
 *
 * The tokenizer stage is a finite state machine.  Each state is
 * implemented as a function with a name that ends in "_state".  The
 * initial state is data_state(). The current tokenizer state is stored
 * in the variable 'tokenizer'.  Most state functions expect a single
 * integer argument which represents a single UTF-16 codepoint.  Some
 * states want more characters and set a lookahead property on
 * themselves.  The scanChars() function in the scanner checks for this
 * lookahead property.  If it doesn't exist, then scanChars() just passes
 * the next input character to the current tokenizer state function.
 * Otherwise, scanChars() looks ahead (a given # of characters, or for a
 * matching string, or for a matching regexp) and passes a string of
 * characters to the current tokenizer state function.
 *
 * As a shortcut, certain states of the tokenizer use regular expressions
 * to look ahead in the scanner's input buffer for runs of text, simple
 * tags and attributes.  For well-formed input, these shortcuts skip a
 * lot of state transitions and speed things up a bit.
 *
 * When a tokenizer state function has consumed a complete token, it
 * emits that token, by calling insertToken(), or by calling a utility
 * function that itself calls insertToken().  These tokens are passed to
 * the tree building stage, which is also a state machine.  Like the
 * tokenizer, the tree building states are implemented as functions, and
 * these functions have names that end with _mode (because the HTML spec
 * refers to them as insertion modes). The current insertion mode is held
 * by the 'parser' variable.  Each insertion mode function takes up to 4
 * arguments.  The first is a token type, represented by the constants
 * TAG, ENDTAG, TEXT, COMMENT, DOCTYPE and EOF.  The second argument is
 * the value of the token: the text or comment data, or tagname or
 * doctype.  For tags, the 3rd argument is an array of attributes.  For
 * DOCTYPES it is the optional public id.  For tags, the 4th argument is
 * true if the tag is self-closing. For doctypes, the 4th argument is the
 * optional system id.
 *
 * Search for "***" to find the major sub-divisions in the code.
 */
const HTMLParser = (function() {

    /***
     * Data prolog.  Lots of constants declared here, including some
     * very large objects.  They're used throughout the code that follows
     */
    // Token types for the tree builder.
    const EOF = -1;
    const TEXT = 1;
    const TAG = 2;
    const ENDTAG = 3;
    const COMMENT = 4;
    const DOCTYPE = 5;

    // A re-usable empty array
    const NOATTRS = Object.freeze([]);

    // These DTD public ids put the browser in quirks mode
    const quirkyPublicIds = /^HTML$|^-\/\/W3O\/\/DTD W3 HTML Strict 3\.0\/\/EN\/\/$|^-\/W3C\/DTD HTML 4\.0 Transitional\/EN$|^\+\/\/Silmaril\/\/dtd html Pro v0r11 19970101\/\/|^-\/\/AdvaSoft Ltd\/\/DTD HTML 3\.0 asWedit \+ extensions\/\/|^-\/\/AS\/\/DTD HTML 3\.0 asWedit \+ extensions\/\/|^-\/\/IETF\/\/DTD HTML 2\.0 Level 1\/\/|^-\/\/IETF\/\/DTD HTML 2\.0 Level 2\/\/|^-\/\/IETF\/\/DTD HTML 2\.0 Strict Level 1\/\/|^-\/\/IETF\/\/DTD HTML 2\.0 Strict Level 2\/\/|^-\/\/IETF\/\/DTD HTML 2\.0 Strict\/\/|^-\/\/IETF\/\/DTD HTML 2\.0\/\/|^-\/\/IETF\/\/DTD HTML 2\.1E\/\/|^-\/\/IETF\/\/DTD HTML 3\.0\/\/|^-\/\/IETF\/\/DTD HTML 3\.2 Final\/\/|^-\/\/IETF\/\/DTD HTML 3\.2\/\/|^-\/\/IETF\/\/DTD HTML 3\/\/|^-\/\/IETF\/\/DTD HTML Level 0\/\/|^-\/\/IETF\/\/DTD HTML Level 1\/\/|^-\/\/IETF\/\/DTD HTML Level 2\/\/|^-\/\/IETF\/\/DTD HTML Level 3\/\/|^-\/\/IETF\/\/DTD HTML Strict Level 0\/\/|^-\/\/IETF\/\/DTD HTML Strict Level 1\/\/|^-\/\/IETF\/\/DTD HTML Strict Level 2\/\/|^-\/\/IETF\/\/DTD HTML Strict Level 3\/\/|^-\/\/IETF\/\/DTD HTML Strict\/\/|^-\/\/IETF\/\/DTD HTML\/\/|^-\/\/Metrius\/\/DTD Metrius Presentational\/\/|^-\/\/Microsoft\/\/DTD Internet Explorer 2\.0 HTML Strict\/\/|^-\/\/Microsoft\/\/DTD Internet Explorer 2\.0 HTML\/\/|^-\/\/Microsoft\/\/DTD Internet Explorer 2\.0 Tables\/\/|^-\/\/Microsoft\/\/DTD Internet Explorer 3\.0 HTML Strict\/\/|^-\/\/Microsoft\/\/DTD Internet Explorer 3\.0 HTML\/\/|^-\/\/Microsoft\/\/DTD Internet Explorer 3\.0 Tables\/\/|^-\/\/Netscape Comm\. Corp\.\/\/DTD HTML\/\/|^-\/\/Netscape Comm\. Corp\.\/\/DTD Strict HTML\/\/|^-\/\/O'Reilly and Associates\/\/DTD HTML 2\.0\/\/|^-\/\/O'Reilly and Associates\/\/DTD HTML Extended 1\.0\/\/|^-\/\/O'Reilly and Associates\/\/DTD HTML Extended Relaxed 1\.0\/\/|^-\/\/SoftQuad Software\/\/DTD HoTMetaL PRO 6\.0::19990601::extensions to HTML 4\.0\/\/|^-\/\/SoftQuad\/\/DTD HoTMetaL PRO 4\.0::19971010::extensions to HTML 4\.0\/\/|^-\/\/Spyglass\/\/DTD HTML 2\.0 Extended\/\/|^-\/\/SQ\/\/DTD HTML 2\.0 HoTMetaL \+ extensions\/\/|^-\/\/Sun Microsystems Corp\.\/\/DTD HotJava HTML\/\/|^-\/\/Sun Microsystems Corp\.\/\/DTD HotJava Strict HTML\/\/|^-\/\/W3C\/\/DTD HTML 3 1995-03-24\/\/|^-\/\/W3C\/\/DTD HTML 3\.2 Draft\/\/|^-\/\/W3C\/\/DTD HTML 3\.2 Final\/\/|^-\/\/W3C\/\/DTD HTML 3\.2\/\/|^-\/\/W3C\/\/DTD HTML 3\.2S Draft\/\/|^-\/\/W3C\/\/DTD HTML 4\.0 Frameset\/\/|^-\/\/W3C\/\/DTD HTML 4\.0 Transitional\/\/|^-\/\/W3C\/\/DTD HTML Experimental 19960712\/\/|^-\/\/W3C\/\/DTD HTML Experimental 970421\/\/|^-\/\/W3C\/\/DTD W3 HTML\/\/|^-\/\/W3O\/\/DTD W3 HTML 3\.0\/\/|^-\/\/WebTechs\/\/DTD Mozilla HTML 2\.0\/\/|^-\/\/WebTechs\/\/DTD Mozilla HTML\/\//i;

    const quirkySystemId = "http://www.ibm.com/data/dtd/v11/ibmxhtml1-transitional.dtd";

    const conditionallyQuirkyPublicIds = /^-\/\/W3C\/\/DTD HTML 4\.01 Frameset\/\/|^-\/\/W3C\/\/DTD HTML 4\.01 Transitional\/\//i;

    // These DTD public ids put the browser in limited quirks mode
    const limitedQuirkyPublicIds = /^-\/\/W3C\/\/DTD XHTML 1\.0 Frameset\/\/|^-\/\/W3C\/\/DTD XHTML 1\.0 Transitional\/\//i;


    // Element sets below.  See the isA() function for a way to test
    // whether an element is a member of a set
    const specialSet = {};
    specialSet[HTML_NAMESPACE] = {
        "address":true, "applet":true, "area":true, "article":true,
        "aside":true, "base":true, "basefont":true, "bgsound":true,
        "blockquote":true, "body":true, "br":true, "button":true,
        "caption":true, "center":true, "col":true, "colgroup":true,
        "command":true, "dd":true, "details":true, "dir":true,
        "div":true, "dl":true, "dt":true, "embed":true,
        "fieldset":true, "figcaption":true, "figure":true, "footer":true,
        "form":true, "frame":true, "frameset":true, "h1":true,
        "h2":true, "h3":true, "h4":true, "h5":true,
        "h6":true, "head":true, "header":true, "hgroup":true,
        "hr":true, "html":true, "iframe":true, "img":true,
        "input":true, "isindex":true, "li":true, "link":true,
        "listing":true, "marquee":true, "menu":true, "meta":true,
        "nav":true, "noembed":true, "noframes":true, "noscript":true,
        "object":true, "ol":true, "p":true, "param":true,
        "plaintext":true, "pre":true, "script":true, "section":true,
        "select":true, "style":true, "summary":true, "table":true,
        "tbody":true, "td":true, "textarea":true, "tfoot":true,
        "th":true, "thead":true, "title":true, "tr":true,
        "ul":true, "wbr":true, "xmp":true
    };
    specialSet[SVG_NAMESPACE] = {
        "foreignObject": true, "desc": true, "title": true
    };
    specialSet[MATHML_NAMESPACE] = {
        "mi":true, "mo":true, "mn":true, "ms":true,
        "mtext":true, "annotation-xml":true
    };
    Object.freeze(specialSet);

    // The set of address, div, and p HTML tags
    const addressdivpSet = {};
    addressdivpSet[HTML_NAMESPACE] = {
        "address":true, "div":true, "p":true
    };
    Object.freeze(addressdivpSet);

    const dddtSet = {};
    dddtSet[HTML_NAMESPACE] = {
        "dd":true, "dt":true
    };
    Object.freeze(dddtSet);

    const tablesectionrowSet = {};
    tablesectionrowSet[HTML_NAMESPACE] = {
        "table":true, "thead":true, "tbody":true, "tfoot":true, "tr":true
    };
    Object.freeze(tablesectionrowSet);

    const impliedEndTagsSet = {};
    impliedEndTagsSet[HTML_NAMESPACE] = {
        "dd": true, "dt": true, "li": true, "option": true,
        "optgroup": true, "p": true, "rp": true, "rt": true
    };
    Object.freeze(impliedEndTagsSet);

    const inScopeSet = {};
    inScopeSet[HTML_NAMESPACE]= {
        "applet":true, "caption":true, "html":true, "table":true,
        "td":true, "th":true, "marquee":true, "object":true
    };
    inScopeSet[MATHML_NAMESPACE] = {
        "mi":true, "mo":true, "mn":true, "ms":true,
        "mtext":true, "annotation-xml":true
    };
    inScopeSet[SVG_NAMESPACE] = {
        "foreignObject":true, "desc":true, "title":true
    };

    const inListItemScopeSet = Object.create(inScopeSet);
    inListItemScopeSet[HTML_NAMESPACE] =
        Object.create(inScopeSet[HTML_NAMESPACE]);
    inListItemScopeSet[HTML_NAMESPACE].ol = true;
    inListItemScopeSet[HTML_NAMESPACE].ul = true;

    const inButtonScopeSet = Object.create(inScopeSet);
    inButtonScopeSet[HTML_NAMESPACE] =
        Object.create(inScopeSet[HTML_NAMESPACE]);
    inButtonScopeSet[HTML_NAMESPACE].button = true;

    Object.freeze(inScopeSet);
    Object.freeze(inListItemScopeSet);
    Object.freeze(inButtonScopeSet);

    const inTableScopeSet = {};
    inTableScopeSet[HTML_NAMESPACE] = {
        "html":true, "table":true
    };
    Object.freeze(inTableScopeSet);

    // The set of elements for select scope is the everything *except* these
    const invertedSelectScopeSet = {};
    invertedSelectScopeSet[HTML_NAMESPACE] = {
        "optgroup":true, "option":true
    };
    Object.freeze(invertedSelectScopeSet);

    const mathmlTextIntegrationPointSet = {};
    mathmlTextIntegrationPointSet[MATHML_NAMESPACE] = {
        mi: true,
        mo: true,
        mn: true,
        ms: true,
        mtext: true
    };
    Object.freeze(mathmlTextIntegrationPointSet);

    const htmlIntegrationPointSet = {};
    htmlIntegrationPointSet[SVG_NAMESPACE] = {
        foreignObject: true,
        desc: true,
        title: true
    };
    Object.freeze(htmlIntegrationPointSet);

    const foreignAttributes = Object.freeze({
        "xlink:actuate": XLINK_NAMESPACE, "xlink:arcrole": XLINK_NAMESPACE,
        "xlink:href":    XLINK_NAMESPACE, "xlink:role":    XLINK_NAMESPACE,
        "xlink:show":    XLINK_NAMESPACE, "xlink:title":   XLINK_NAMESPACE,
        "xlink:type":    XLINK_NAMESPACE, "xml:base":      XML_NAMESPACE,
        "xml:lang":      XML_NAMESPACE,   "xml:space":     XML_NAMESPACE,
        "xmlns":         XMLNS_NAMESPACE, "xmlns:xlink":   XMLNS_NAMESPACE
    });


    // Lowercase to mixed case mapping for SVG attributes and tagnames
    const svgAttrAdjustments = Object.freeze({
        attributename: "attributeName", attributetype: "attributeType",
        basefrequency: "baseFrequency", baseprofile: "baseProfile",
        calcmode: "calcMode", clippathunits: "clipPathUnits",
        contentscripttype: "contentScriptType",
        contentstyletype: "contentStyleType",
        diffuseconstant: "diffuseConstant",
        edgemode: "edgeMode",
        externalresourcesrequired: "externalResourcesRequired",
        filterres: "filterRes", filterunits: "filterUnits",
        glyphref: "glyphRef", gradienttransform: "gradientTransform",
        gradientunits: "gradientUnits", kernelmatrix: "kernelMatrix",
        kernelunitlength: "kernelUnitLength", keypoints: "keyPoints",
        keysplines: "keySplines", keytimes: "keyTimes",
        lengthadjust: "lengthAdjust", limitingconeangle: "limitingConeAngle",
        markerheight: "markerHeight", markerunits: "markerUnits",
        markerwidth: "markerWidth", maskcontentunits: "maskContentUnits",
        maskunits: "maskUnits", numoctaves: "numOctaves",
        pathlength: "pathLength", patterncontentunits: "patternContentUnits",
        patterntransform: "patternTransform", patternunits: "patternUnits",
        pointsatx: "pointsAtX", pointsaty: "pointsAtY",
        pointsatz: "pointsAtZ", preservealpha: "preserveAlpha",
        preserveaspectratio: "preserveAspectRatio",
        primitiveunits: "primitiveUnits", refx: "refX",
        refy: "refY", repeatcount: "repeatCount",
        repeatdur: "repeatDur", requiredextensions: "requiredExtensions",
        requiredfeatures: "requiredFeatures",
        specularconstant: "specularConstant",
        specularexponent: "specularExponent", spreadmethod: "spreadMethod",
        startoffset: "startOffset", stddeviation: "stdDeviation",
        stitchtiles: "stitchTiles", surfacescale: "surfaceScale",
        systemlanguage: "systemLanguage", tablevalues: "tableValues",
        targetx: "targetX", targety: "targetY",
        textlength: "textLength", viewbox: "viewBox",
        viewtarget: "viewTarget", xchannelselector: "xChannelSelector",
        ychannelselector: "yChannelSelector", zoomandpan: "zoomAndPan"
    });

    const svgTagNameAdjustments = Object.freeze({
        altglyph: "altGlyph", altglyphdef: "altGlyphDef",
        altglyphitem: "altGlyphItem", animatecolor: "animateColor",
        animatemotion: "animateMotion", animatetransform: "animateTransform",
        clippath: "clipPath", feblend: "feBlend",
        fecolormatrix: "feColorMatrix",
        fecomponenttransfer: "feComponentTransfer", fecomposite: "feComposite",
        feconvolvematrix: "feConvolveMatrix",
        fediffuselighting: "feDiffuseLighting",
        fedisplacementmap: "feDisplacementMap",
        fedistantlight: "feDistantLight", feflood: "feFlood",
        fefunca: "feFuncA", fefuncb: "feFuncB",
        fefuncg: "feFuncG", fefuncr: "feFuncR",
        fegaussianblur: "feGaussianBlur", feimage: "feImage",
        femerge: "feMerge", femergenode: "feMergeNode",
        femorphology: "feMorphology", feoffset: "feOffset",
        fepointlight: "fePointLight", fespecularlighting: "feSpecularLighting",
        fespotlight: "feSpotLight", fetile: "feTile",
        feturbulence: "feTurbulence", foreignobject: "foreignObject",
        glyphref: "glyphRef", lineargradient: "linearGradient",
        radialgradient: "radialGradient", textpath: "textPath"
    });


    // Data for parsing numeric and named character references
    // These next 3 objects are direct translations of tables
    // in the HTML spec into JavaScript object format
    const numericCharRefReplacements = Object.freeze({
        0x00:0xFFFD, 0x80:0x20AC, 0x82:0x201A, 0x83:0x0192, 0x84:0x201E,
        0x85:0x2026, 0x86:0x2020, 0x87:0x2021, 0x88:0x02C6, 0x89:0x2030,
        0x8A:0x0160, 0x8B:0x2039, 0x8C:0x0152, 0x8E:0x017D, 0x91:0x2018,
        0x92:0x2019, 0x93:0x201C, 0x94:0x201D, 0x95:0x2022, 0x96:0x2013,
        0x97:0x2014, 0x98:0x02DC, 0x99:0x2122, 0x9A:0x0161, 0x9B:0x203A,
        0x9C:0x0153, 0x9E:0x017E, 0x9F:0x0178
    });

    // These named character references work even without the semicolon
    const namedCharRefsNoSemi = Object.freeze({
        "AElig":0xC6, "AMP":0x26, "Aacute":0xC1, "Acirc":0xC2,
        "Agrave":0xC0, "Aring":0xC5, "Atilde":0xC3, "Auml":0xC4,
        "COPY":0xA9, "Ccedil":0xC7, "ETH":0xD0, "Eacute":0xC9,
        "Ecirc":0xCA, "Egrave":0xC8, "Euml":0xCB, "GT":0x3E,
        "Iacute":0xCD, "Icirc":0xCE, "Igrave":0xCC, "Iuml":0xCF,
        "LT":0x3C, "Ntilde":0xD1, "Oacute":0xD3, "Ocirc":0xD4,
        "Ograve":0xD2, "Oslash":0xD8, "Otilde":0xD5, "Ouml":0xD6,
        "QUOT":0x22, "REG":0xAE, "THORN":0xDE, "Uacute":0xDA,
        "Ucirc":0xDB, "Ugrave":0xD9, "Uuml":0xDC, "Yacute":0xDD,
        "aacute":0xE1, "acirc":0xE2, "acute":0xB4, "aelig":0xE6,
        "agrave":0xE0, "amp":0x26, "aring":0xE5, "atilde":0xE3,
        "auml":0xE4, "brvbar":0xA6, "ccedil":0xE7, "cedil":0xB8,
        "cent":0xA2, "copy":0xA9, "curren":0xA4, "deg":0xB0,
        "divide":0xF7, "eacute":0xE9, "ecirc":0xEA, "egrave":0xE8,
        "eth":0xF0, "euml":0xEB, "frac12":0xBD, "frac14":0xBC,
        "frac34":0xBE, "gt":0x3E, "iacute":0xED, "icirc":0xEE,
        "iexcl":0xA1, "igrave":0xEC, "iquest":0xBF, "iuml":0xEF,
        "laquo":0xAB, "lt":0x3C, "macr":0xAF, "micro":0xB5,
        "middot":0xB7, "nbsp":0xA0, "not":0xAC, "ntilde":0xF1,
        "oacute":0xF3, "ocirc":0xF4, "ograve":0xF2, "ordf":0xAA,
        "ordm":0xBA, "oslash":0xF8, "otilde":0xF5, "ouml":0xF6,
        "para":0xB6, "plusmn":0xB1, "pound":0xA3, "quot":0x22,
        "raquo":0xBB, "reg":0xAE, "sect":0xA7, "shy":0xAD,
        "sup1":0xB9, "sup2":0xB2, "sup3":0xB3, "szlig":0xDF,
        "thorn":0xFE, "times":0xD7, "uacute":0xFA, "ucirc":0xFB,
        "ugrave":0xF9, "uml":0xA8, "uuml":0xFC, "yacute":0xFD,
        "yen":0xA5, "yuml":0xFF
    });

    const namedCharRefs = Object.freeze({
        "AElig;":0xc6, "AMP;":0x26,
        "Aacute;":0xc1, "Abreve;":0x102,
        "Acirc;":0xc2, "Acy;":0x410,
        "Afr;":[0xd835,0xdd04], "Agrave;":0xc0,
        "Alpha;":0x391, "Amacr;":0x100,
        "And;":0x2a53, "Aogon;":0x104,
        "Aopf;":[0xd835,0xdd38], "ApplyFunction;":0x2061,
        "Aring;":0xc5, "Ascr;":[0xd835,0xdc9c],
        "Assign;":0x2254, "Atilde;":0xc3,
        "Auml;":0xc4, "Backslash;":0x2216,
        "Barv;":0x2ae7, "Barwed;":0x2306,
        "Bcy;":0x411, "Because;":0x2235,
        "Bernoullis;":0x212c, "Beta;":0x392,
        "Bfr;":[0xd835,0xdd05], "Bopf;":[0xd835,0xdd39],
        "Breve;":0x2d8, "Bscr;":0x212c,
        "Bumpeq;":0x224e, "CHcy;":0x427,
        "COPY;":0xa9, "Cacute;":0x106,
        "Cap;":0x22d2, "CapitalDifferentialD;":0x2145,
        "Cayleys;":0x212d, "Ccaron;":0x10c,
        "Ccedil;":0xc7, "Ccirc;":0x108,
        "Cconint;":0x2230, "Cdot;":0x10a,
        "Cedilla;":0xb8, "CenterDot;":0xb7,
        "Cfr;":0x212d, "Chi;":0x3a7,
        "CircleDot;":0x2299, "CircleMinus;":0x2296,
        "CirclePlus;":0x2295, "CircleTimes;":0x2297,
        "ClockwiseContourIntegral;":0x2232, "CloseCurlyDoubleQuote;":0x201d,
        "CloseCurlyQuote;":0x2019, "Colon;":0x2237,
        "Colone;":0x2a74, "Congruent;":0x2261,
        "Conint;":0x222f, "ContourIntegral;":0x222e,
        "Copf;":0x2102, "Coproduct;":0x2210,
        "CounterClockwiseContourIntegral;":0x2233, "Cross;":0x2a2f,
        "Cscr;":[0xd835,0xdc9e], "Cup;":0x22d3,
        "CupCap;":0x224d, "DD;":0x2145,
        "DDotrahd;":0x2911, "DJcy;":0x402,
        "DScy;":0x405, "DZcy;":0x40f,
        "Dagger;":0x2021, "Darr;":0x21a1,
        "Dashv;":0x2ae4, "Dcaron;":0x10e,
        "Dcy;":0x414, "Del;":0x2207,
        "Delta;":0x394, "Dfr;":[0xd835,0xdd07],
        "DiacriticalAcute;":0xb4, "DiacriticalDot;":0x2d9,
        "DiacriticalDoubleAcute;":0x2dd, "DiacriticalGrave;":0x60,
        "DiacriticalTilde;":0x2dc, "Diamond;":0x22c4,
        "DifferentialD;":0x2146, "Dopf;":[0xd835,0xdd3b],
        "Dot;":0xa8, "DotDot;":0x20dc,
        "DotEqual;":0x2250, "DoubleContourIntegral;":0x222f,
        "DoubleDot;":0xa8, "DoubleDownArrow;":0x21d3,
        "DoubleLeftArrow;":0x21d0, "DoubleLeftRightArrow;":0x21d4,
        "DoubleLeftTee;":0x2ae4, "DoubleLongLeftArrow;":0x27f8,
        "DoubleLongLeftRightArrow;":0x27fa, "DoubleLongRightArrow;":0x27f9,
        "DoubleRightArrow;":0x21d2, "DoubleRightTee;":0x22a8,
        "DoubleUpArrow;":0x21d1, "DoubleUpDownArrow;":0x21d5,
        "DoubleVerticalBar;":0x2225, "DownArrow;":0x2193,
        "DownArrowBar;":0x2913, "DownArrowUpArrow;":0x21f5,
        "DownBreve;":0x311, "DownLeftRightVector;":0x2950,
        "DownLeftTeeVector;":0x295e, "DownLeftVector;":0x21bd,
        "DownLeftVectorBar;":0x2956, "DownRightTeeVector;":0x295f,
        "DownRightVector;":0x21c1, "DownRightVectorBar;":0x2957,
        "DownTee;":0x22a4, "DownTeeArrow;":0x21a7,
        "Downarrow;":0x21d3, "Dscr;":[0xd835,0xdc9f],
        "Dstrok;":0x110, "ENG;":0x14a,
        "ETH;":0xd0, "Eacute;":0xc9,
        "Ecaron;":0x11a, "Ecirc;":0xca,
        "Ecy;":0x42d, "Edot;":0x116,
        "Efr;":[0xd835,0xdd08], "Egrave;":0xc8,
        "Element;":0x2208, "Emacr;":0x112,
        "EmptySmallSquare;":0x25fb, "EmptyVerySmallSquare;":0x25ab,
        "Eogon;":0x118, "Eopf;":[0xd835,0xdd3c],
        "Epsilon;":0x395, "Equal;":0x2a75,
        "EqualTilde;":0x2242, "Equilibrium;":0x21cc,
        "Escr;":0x2130, "Esim;":0x2a73,
        "Eta;":0x397, "Euml;":0xcb,
        "Exists;":0x2203, "ExponentialE;":0x2147,
        "Fcy;":0x424, "Ffr;":[0xd835,0xdd09],
        "FilledSmallSquare;":0x25fc, "FilledVerySmallSquare;":0x25aa,
        "Fopf;":[0xd835,0xdd3d], "ForAll;":0x2200,
        "Fouriertrf;":0x2131, "Fscr;":0x2131,
        "GJcy;":0x403, "GT;":0x3e,
        "Gamma;":0x393, "Gammad;":0x3dc,
        "Gbreve;":0x11e, "Gcedil;":0x122,
        "Gcirc;":0x11c, "Gcy;":0x413,
        "Gdot;":0x120, "Gfr;":[0xd835,0xdd0a],
        "Gg;":0x22d9, "Gopf;":[0xd835,0xdd3e],
        "GreaterEqual;":0x2265, "GreaterEqualLess;":0x22db,
        "GreaterFullEqual;":0x2267, "GreaterGreater;":0x2aa2,
        "GreaterLess;":0x2277, "GreaterSlantEqual;":0x2a7e,
        "GreaterTilde;":0x2273, "Gscr;":[0xd835,0xdca2],
        "Gt;":0x226b, "HARDcy;":0x42a,
        "Hacek;":0x2c7, "Hat;":0x5e,
        "Hcirc;":0x124, "Hfr;":0x210c,
        "HilbertSpace;":0x210b, "Hopf;":0x210d,
        "HorizontalLine;":0x2500, "Hscr;":0x210b,
        "Hstrok;":0x126, "HumpDownHump;":0x224e,
        "HumpEqual;":0x224f, "IEcy;":0x415,
        "IJlig;":0x132, "IOcy;":0x401,
        "Iacute;":0xcd, "Icirc;":0xce,
        "Icy;":0x418, "Idot;":0x130,
        "Ifr;":0x2111, "Igrave;":0xcc,
        "Im;":0x2111, "Imacr;":0x12a,
        "ImaginaryI;":0x2148, "Implies;":0x21d2,
        "Int;":0x222c, "Integral;":0x222b,
        "Intersection;":0x22c2, "InvisibleComma;":0x2063,
        "InvisibleTimes;":0x2062, "Iogon;":0x12e,
        "Iopf;":[0xd835,0xdd40], "Iota;":0x399,
        "Iscr;":0x2110, "Itilde;":0x128,
        "Iukcy;":0x406, "Iuml;":0xcf,
        "Jcirc;":0x134, "Jcy;":0x419,
        "Jfr;":[0xd835,0xdd0d], "Jopf;":[0xd835,0xdd41],
        "Jscr;":[0xd835,0xdca5], "Jsercy;":0x408,
        "Jukcy;":0x404, "KHcy;":0x425,
        "KJcy;":0x40c, "Kappa;":0x39a,
        "Kcedil;":0x136, "Kcy;":0x41a,
        "Kfr;":[0xd835,0xdd0e], "Kopf;":[0xd835,0xdd42],
        "Kscr;":[0xd835,0xdca6], "LJcy;":0x409,
        "LT;":0x3c, "Lacute;":0x139,
        "Lambda;":0x39b, "Lang;":0x27ea,
        "Laplacetrf;":0x2112, "Larr;":0x219e,
        "Lcaron;":0x13d, "Lcedil;":0x13b,
        "Lcy;":0x41b, "LeftAngleBracket;":0x27e8,
        "LeftArrow;":0x2190, "LeftArrowBar;":0x21e4,
        "LeftArrowRightArrow;":0x21c6, "LeftCeiling;":0x2308,
        "LeftDoubleBracket;":0x27e6, "LeftDownTeeVector;":0x2961,
        "LeftDownVector;":0x21c3, "LeftDownVectorBar;":0x2959,
        "LeftFloor;":0x230a, "LeftRightArrow;":0x2194,
        "LeftRightVector;":0x294e, "LeftTee;":0x22a3,
        "LeftTeeArrow;":0x21a4, "LeftTeeVector;":0x295a,
        "LeftTriangle;":0x22b2, "LeftTriangleBar;":0x29cf,
        "LeftTriangleEqual;":0x22b4, "LeftUpDownVector;":0x2951,
        "LeftUpTeeVector;":0x2960, "LeftUpVector;":0x21bf,
        "LeftUpVectorBar;":0x2958, "LeftVector;":0x21bc,
        "LeftVectorBar;":0x2952, "Leftarrow;":0x21d0,
        "Leftrightarrow;":0x21d4, "LessEqualGreater;":0x22da,
        "LessFullEqual;":0x2266, "LessGreater;":0x2276,
        "LessLess;":0x2aa1, "LessSlantEqual;":0x2a7d,
        "LessTilde;":0x2272, "Lfr;":[0xd835,0xdd0f],
        "Ll;":0x22d8, "Lleftarrow;":0x21da,
        "Lmidot;":0x13f, "LongLeftArrow;":0x27f5,
        "LongLeftRightArrow;":0x27f7, "LongRightArrow;":0x27f6,
        "Longleftarrow;":0x27f8, "Longleftrightarrow;":0x27fa,
        "Longrightarrow;":0x27f9, "Lopf;":[0xd835,0xdd43],
        "LowerLeftArrow;":0x2199, "LowerRightArrow;":0x2198,
        "Lscr;":0x2112, "Lsh;":0x21b0,
        "Lstrok;":0x141, "Lt;":0x226a,
        "Map;":0x2905, "Mcy;":0x41c,
        "MediumSpace;":0x205f, "Mellintrf;":0x2133,
        "Mfr;":[0xd835,0xdd10], "MinusPlus;":0x2213,
        "Mopf;":[0xd835,0xdd44], "Mscr;":0x2133,
        "Mu;":0x39c, "NJcy;":0x40a,
        "Nacute;":0x143, "Ncaron;":0x147,
        "Ncedil;":0x145, "Ncy;":0x41d,
        "NegativeMediumSpace;":0x200b, "NegativeThickSpace;":0x200b,
        "NegativeThinSpace;":0x200b, "NegativeVeryThinSpace;":0x200b,
        "NestedGreaterGreater;":0x226b, "NestedLessLess;":0x226a,
        "NewLine;":0xa, "Nfr;":[0xd835,0xdd11],
        "NoBreak;":0x2060, "NonBreakingSpace;":0xa0,
        "Nopf;":0x2115, "Not;":0x2aec,
        "NotCongruent;":0x2262, "NotCupCap;":0x226d,
        "NotDoubleVerticalBar;":0x2226, "NotElement;":0x2209,
        "NotEqual;":0x2260, "NotEqualTilde;":[0x2242,0x338],
        "NotExists;":0x2204, "NotGreater;":0x226f,
        "NotGreaterEqual;":0x2271, "NotGreaterFullEqual;":[0x2267,0x338],
        "NotGreaterGreater;":[0x226b,0x338], "NotGreaterLess;":0x2279,
        "NotGreaterSlantEqual;":[0x2a7e,0x338], "NotGreaterTilde;":0x2275,
        "NotHumpDownHump;":[0x224e,0x338], "NotHumpEqual;":[0x224f,0x338],
        "NotLeftTriangle;":0x22ea, "NotLeftTriangleBar;":[0x29cf,0x338],
        "NotLeftTriangleEqual;":0x22ec, "NotLess;":0x226e,
        "NotLessEqual;":0x2270, "NotLessGreater;":0x2278,
        "NotLessLess;":[0x226a,0x338], "NotLessSlantEqual;":[0x2a7d,0x338],
        "NotLessTilde;":0x2274, "NotNestedGreaterGreater;":[0x2aa2,0x338],
        "NotNestedLessLess;":[0x2aa1,0x338], "NotPrecedes;":0x2280,
        "NotPrecedesEqual;":[0x2aaf,0x338], "NotPrecedesSlantEqual;":0x22e0,
        "NotReverseElement;":0x220c, "NotRightTriangle;":0x22eb,
        "NotRightTriangleBar;":[0x29d0,0x338], "NotRightTriangleEqual;":0x22ed,
        "NotSquareSubset;":[0x228f,0x338], "NotSquareSubsetEqual;":0x22e2,
        "NotSquareSuperset;":[0x2290,0x338], "NotSquareSupersetEqual;":0x22e3,
        "NotSubset;":[0x2282,0x20d2], "NotSubsetEqual;":0x2288,
        "NotSucceeds;":0x2281, "NotSucceedsEqual;":[0x2ab0,0x338],
        "NotSucceedsSlantEqual;":0x22e1, "NotSucceedsTilde;":[0x227f,0x338],
        "NotSuperset;":[0x2283,0x20d2], "NotSupersetEqual;":0x2289,
        "NotTilde;":0x2241, "NotTildeEqual;":0x2244,
        "NotTildeFullEqual;":0x2247, "NotTildeTilde;":0x2249,
        "NotVerticalBar;":0x2224, "Nscr;":[0xd835,0xdca9],
        "Ntilde;":0xd1, "Nu;":0x39d,
        "OElig;":0x152, "Oacute;":0xd3,
        "Ocirc;":0xd4, "Ocy;":0x41e,
        "Odblac;":0x150, "Ofr;":[0xd835,0xdd12],
        "Ograve;":0xd2, "Omacr;":0x14c,
        "Omega;":0x3a9, "Omicron;":0x39f,
        "Oopf;":[0xd835,0xdd46], "OpenCurlyDoubleQuote;":0x201c,
        "OpenCurlyQuote;":0x2018, "Or;":0x2a54,
        "Oscr;":[0xd835,0xdcaa], "Oslash;":0xd8,
        "Otilde;":0xd5, "Otimes;":0x2a37,
        "Ouml;":0xd6, "OverBar;":0x203e,
        "OverBrace;":0x23de, "OverBracket;":0x23b4,
        "OverParenthesis;":0x23dc, "PartialD;":0x2202,
        "Pcy;":0x41f, "Pfr;":[0xd835,0xdd13],
        "Phi;":0x3a6, "Pi;":0x3a0,
        "PlusMinus;":0xb1, "Poincareplane;":0x210c,
        "Popf;":0x2119, "Pr;":0x2abb,
        "Precedes;":0x227a, "PrecedesEqual;":0x2aaf,
        "PrecedesSlantEqual;":0x227c, "PrecedesTilde;":0x227e,
        "Prime;":0x2033, "Product;":0x220f,
        "Proportion;":0x2237, "Proportional;":0x221d,
        "Pscr;":[0xd835,0xdcab], "Psi;":0x3a8,
        "QUOT;":0x22, "Qfr;":[0xd835,0xdd14],
        "Qopf;":0x211a, "Qscr;":[0xd835,0xdcac],
        "RBarr;":0x2910, "REG;":0xae,
        "Racute;":0x154, "Rang;":0x27eb,
        "Rarr;":0x21a0, "Rarrtl;":0x2916,
        "Rcaron;":0x158, "Rcedil;":0x156,
        "Rcy;":0x420, "Re;":0x211c,
        "ReverseElement;":0x220b, "ReverseEquilibrium;":0x21cb,
        "ReverseUpEquilibrium;":0x296f, "Rfr;":0x211c,
        "Rho;":0x3a1, "RightAngleBracket;":0x27e9,
        "RightArrow;":0x2192, "RightArrowBar;":0x21e5,
        "RightArrowLeftArrow;":0x21c4, "RightCeiling;":0x2309,
        "RightDoubleBracket;":0x27e7, "RightDownTeeVector;":0x295d,
        "RightDownVector;":0x21c2, "RightDownVectorBar;":0x2955,
        "RightFloor;":0x230b, "RightTee;":0x22a2,
        "RightTeeArrow;":0x21a6, "RightTeeVector;":0x295b,
        "RightTriangle;":0x22b3, "RightTriangleBar;":0x29d0,
        "RightTriangleEqual;":0x22b5, "RightUpDownVector;":0x294f,
        "RightUpTeeVector;":0x295c, "RightUpVector;":0x21be,
        "RightUpVectorBar;":0x2954, "RightVector;":0x21c0,
        "RightVectorBar;":0x2953, "Rightarrow;":0x21d2,
        "Ropf;":0x211d, "RoundImplies;":0x2970,
        "Rrightarrow;":0x21db, "Rscr;":0x211b,
        "Rsh;":0x21b1, "RuleDelayed;":0x29f4,
        "SHCHcy;":0x429, "SHcy;":0x428,
        "SOFTcy;":0x42c, "Sacute;":0x15a,
        "Sc;":0x2abc, "Scaron;":0x160,
        "Scedil;":0x15e, "Scirc;":0x15c,
        "Scy;":0x421, "Sfr;":[0xd835,0xdd16],
        "ShortDownArrow;":0x2193, "ShortLeftArrow;":0x2190,
        "ShortRightArrow;":0x2192, "ShortUpArrow;":0x2191,
        "Sigma;":0x3a3, "SmallCircle;":0x2218,
        "Sopf;":[0xd835,0xdd4a], "Sqrt;":0x221a,
        "Square;":0x25a1, "SquareIntersection;":0x2293,
        "SquareSubset;":0x228f, "SquareSubsetEqual;":0x2291,
        "SquareSuperset;":0x2290, "SquareSupersetEqual;":0x2292,
        "SquareUnion;":0x2294, "Sscr;":[0xd835,0xdcae],
        "Star;":0x22c6, "Sub;":0x22d0,
        "Subset;":0x22d0, "SubsetEqual;":0x2286,
        "Succeeds;":0x227b, "SucceedsEqual;":0x2ab0,
        "SucceedsSlantEqual;":0x227d, "SucceedsTilde;":0x227f,
        "SuchThat;":0x220b, "Sum;":0x2211,
        "Sup;":0x22d1, "Superset;":0x2283,
        "SupersetEqual;":0x2287, "Supset;":0x22d1,
        "THORN;":0xde, "TRADE;":0x2122,
        "TSHcy;":0x40b, "TScy;":0x426,
        "Tab;":0x9, "Tau;":0x3a4,
        "Tcaron;":0x164, "Tcedil;":0x162,
        "Tcy;":0x422, "Tfr;":[0xd835,0xdd17],
        "Therefore;":0x2234, "Theta;":0x398,
        "ThickSpace;":[0x205f,0x200a], "ThinSpace;":0x2009,
        "Tilde;":0x223c, "TildeEqual;":0x2243,
        "TildeFullEqual;":0x2245, "TildeTilde;":0x2248,
        "Topf;":[0xd835,0xdd4b], "TripleDot;":0x20db,
        "Tscr;":[0xd835,0xdcaf], "Tstrok;":0x166,
        "Uacute;":0xda, "Uarr;":0x219f,
        "Uarrocir;":0x2949, "Ubrcy;":0x40e,
        "Ubreve;":0x16c, "Ucirc;":0xdb,
        "Ucy;":0x423, "Udblac;":0x170,
        "Ufr;":[0xd835,0xdd18], "Ugrave;":0xd9,
        "Umacr;":0x16a, "UnderBar;":0x5f,
        "UnderBrace;":0x23df, "UnderBracket;":0x23b5,
        "UnderParenthesis;":0x23dd, "Union;":0x22c3,
        "UnionPlus;":0x228e, "Uogon;":0x172,
        "Uopf;":[0xd835,0xdd4c], "UpArrow;":0x2191,
        "UpArrowBar;":0x2912, "UpArrowDownArrow;":0x21c5,
        "UpDownArrow;":0x2195, "UpEquilibrium;":0x296e,
        "UpTee;":0x22a5, "UpTeeArrow;":0x21a5,
        "Uparrow;":0x21d1, "Updownarrow;":0x21d5,
        "UpperLeftArrow;":0x2196, "UpperRightArrow;":0x2197,
        "Upsi;":0x3d2, "Upsilon;":0x3a5,
        "Uring;":0x16e, "Uscr;":[0xd835,0xdcb0],
        "Utilde;":0x168, "Uuml;":0xdc,
        "VDash;":0x22ab, "Vbar;":0x2aeb,
        "Vcy;":0x412, "Vdash;":0x22a9,
        "Vdashl;":0x2ae6, "Vee;":0x22c1,
        "Verbar;":0x2016, "Vert;":0x2016,
        "VerticalBar;":0x2223, "VerticalLine;":0x7c,
        "VerticalSeparator;":0x2758, "VerticalTilde;":0x2240,
        "VeryThinSpace;":0x200a, "Vfr;":[0xd835,0xdd19],
        "Vopf;":[0xd835,0xdd4d], "Vscr;":[0xd835,0xdcb1],
        "Vvdash;":0x22aa, "Wcirc;":0x174,
        "Wedge;":0x22c0, "Wfr;":[0xd835,0xdd1a],
        "Wopf;":[0xd835,0xdd4e], "Wscr;":[0xd835,0xdcb2],
        "Xfr;":[0xd835,0xdd1b], "Xi;":0x39e,
        "Xopf;":[0xd835,0xdd4f], "Xscr;":[0xd835,0xdcb3],
        "YAcy;":0x42f, "YIcy;":0x407,
        "YUcy;":0x42e, "Yacute;":0xdd,
        "Ycirc;":0x176, "Ycy;":0x42b,
        "Yfr;":[0xd835,0xdd1c], "Yopf;":[0xd835,0xdd50],
        "Yscr;":[0xd835,0xdcb4], "Yuml;":0x178,
        "ZHcy;":0x416, "Zacute;":0x179,
        "Zcaron;":0x17d, "Zcy;":0x417,
        "Zdot;":0x17b, "ZeroWidthSpace;":0x200b,
        "Zeta;":0x396, "Zfr;":0x2128,
        "Zopf;":0x2124, "Zscr;":[0xd835,0xdcb5],
        "aacute;":0xe1, "abreve;":0x103,
        "ac;":0x223e, "acE;":[0x223e,0x333],
        "acd;":0x223f, "acirc;":0xe2,
        "acute;":0xb4, "acy;":0x430,
        "aelig;":0xe6, "af;":0x2061,
        "afr;":[0xd835,0xdd1e], "agrave;":0xe0,
        "alefsym;":0x2135, "aleph;":0x2135,
        "alpha;":0x3b1, "amacr;":0x101,
        "amalg;":0x2a3f, "amp;":0x26,
        "and;":0x2227, "andand;":0x2a55,
        "andd;":0x2a5c, "andslope;":0x2a58,
        "andv;":0x2a5a, "ang;":0x2220,
        "ange;":0x29a4, "angle;":0x2220,
        "angmsd;":0x2221, "angmsdaa;":0x29a8,
        "angmsdab;":0x29a9, "angmsdac;":0x29aa,
        "angmsdad;":0x29ab, "angmsdae;":0x29ac,
        "angmsdaf;":0x29ad, "angmsdag;":0x29ae,
        "angmsdah;":0x29af, "angrt;":0x221f,
        "angrtvb;":0x22be, "angrtvbd;":0x299d,
        "angsph;":0x2222, "angst;":0xc5,
        "angzarr;":0x237c, "aogon;":0x105,
        "aopf;":[0xd835,0xdd52], "ap;":0x2248,
        "apE;":0x2a70, "apacir;":0x2a6f,
        "ape;":0x224a, "apid;":0x224b,
        "apos;":0x27, "approx;":0x2248,
        "approxeq;":0x224a, "aring;":0xe5,
        "ascr;":[0xd835,0xdcb6], "ast;":0x2a,
        "asymp;":0x2248, "asympeq;":0x224d,
        "atilde;":0xe3, "auml;":0xe4,
        "awconint;":0x2233, "awint;":0x2a11,
        "bNot;":0x2aed, "backcong;":0x224c,
        "backepsilon;":0x3f6, "backprime;":0x2035,
        "backsim;":0x223d, "backsimeq;":0x22cd,
        "barvee;":0x22bd, "barwed;":0x2305,
        "barwedge;":0x2305, "bbrk;":0x23b5,
        "bbrktbrk;":0x23b6, "bcong;":0x224c,
        "bcy;":0x431, "bdquo;":0x201e,
        "becaus;":0x2235, "because;":0x2235,
        "bemptyv;":0x29b0, "bepsi;":0x3f6,
        "bernou;":0x212c, "beta;":0x3b2,
        "beth;":0x2136, "between;":0x226c,
        "bfr;":[0xd835,0xdd1f], "bigcap;":0x22c2,
        "bigcirc;":0x25ef, "bigcup;":0x22c3,
        "bigodot;":0x2a00, "bigoplus;":0x2a01,
        "bigotimes;":0x2a02, "bigsqcup;":0x2a06,
        "bigstar;":0x2605, "bigtriangledown;":0x25bd,
        "bigtriangleup;":0x25b3, "biguplus;":0x2a04,
        "bigvee;":0x22c1, "bigwedge;":0x22c0,
        "bkarow;":0x290d, "blacklozenge;":0x29eb,
        "blacksquare;":0x25aa, "blacktriangle;":0x25b4,
        "blacktriangledown;":0x25be, "blacktriangleleft;":0x25c2,
        "blacktriangleright;":0x25b8, "blank;":0x2423,
        "blk12;":0x2592, "blk14;":0x2591,
        "blk34;":0x2593, "block;":0x2588,
        "bne;":[0x3d,0x20e5], "bnequiv;":[0x2261,0x20e5],
        "bnot;":0x2310, "bopf;":[0xd835,0xdd53],
        "bot;":0x22a5, "bottom;":0x22a5,
        "bowtie;":0x22c8, "boxDL;":0x2557,
        "boxDR;":0x2554, "boxDl;":0x2556,
        "boxDr;":0x2553, "boxH;":0x2550,
        "boxHD;":0x2566, "boxHU;":0x2569,
        "boxHd;":0x2564, "boxHu;":0x2567,
        "boxUL;":0x255d, "boxUR;":0x255a,
        "boxUl;":0x255c, "boxUr;":0x2559,
        "boxV;":0x2551, "boxVH;":0x256c,
        "boxVL;":0x2563, "boxVR;":0x2560,
        "boxVh;":0x256b, "boxVl;":0x2562,
        "boxVr;":0x255f, "boxbox;":0x29c9,
        "boxdL;":0x2555, "boxdR;":0x2552,
        "boxdl;":0x2510, "boxdr;":0x250c,
        "boxh;":0x2500, "boxhD;":0x2565,
        "boxhU;":0x2568, "boxhd;":0x252c,
        "boxhu;":0x2534, "boxminus;":0x229f,
        "boxplus;":0x229e, "boxtimes;":0x22a0,
        "boxuL;":0x255b, "boxuR;":0x2558,
        "boxul;":0x2518, "boxur;":0x2514,
        "boxv;":0x2502, "boxvH;":0x256a,
        "boxvL;":0x2561, "boxvR;":0x255e,
        "boxvh;":0x253c, "boxvl;":0x2524,
        "boxvr;":0x251c, "bprime;":0x2035,
        "breve;":0x2d8, "brvbar;":0xa6,
        "bscr;":[0xd835,0xdcb7], "bsemi;":0x204f,
        "bsim;":0x223d, "bsime;":0x22cd,
        "bsol;":0x5c, "bsolb;":0x29c5,
        "bsolhsub;":0x27c8, "bull;":0x2022,
        "bullet;":0x2022, "bump;":0x224e,
        "bumpE;":0x2aae, "bumpe;":0x224f,
        "bumpeq;":0x224f, "cacute;":0x107,
        "cap;":0x2229, "capand;":0x2a44,
        "capbrcup;":0x2a49, "capcap;":0x2a4b,
        "capcup;":0x2a47, "capdot;":0x2a40,
        "caps;":[0x2229,0xfe00], "caret;":0x2041,
        "caron;":0x2c7, "ccaps;":0x2a4d,
        "ccaron;":0x10d, "ccedil;":0xe7,
        "ccirc;":0x109, "ccups;":0x2a4c,
        "ccupssm;":0x2a50, "cdot;":0x10b,
        "cedil;":0xb8, "cemptyv;":0x29b2,
        "cent;":0xa2, "centerdot;":0xb7,
        "cfr;":[0xd835,0xdd20], "chcy;":0x447,
        "check;":0x2713, "checkmark;":0x2713,
        "chi;":0x3c7, "cir;":0x25cb,
        "cirE;":0x29c3, "circ;":0x2c6,
        "circeq;":0x2257, "circlearrowleft;":0x21ba,
        "circlearrowright;":0x21bb, "circledR;":0xae,
        "circledS;":0x24c8, "circledast;":0x229b,
        "circledcirc;":0x229a, "circleddash;":0x229d,
        "cire;":0x2257, "cirfnint;":0x2a10,
        "cirmid;":0x2aef, "cirscir;":0x29c2,
        "clubs;":0x2663, "clubsuit;":0x2663,
        "colon;":0x3a, "colone;":0x2254,
        "coloneq;":0x2254, "comma;":0x2c,
        "commat;":0x40, "comp;":0x2201,
        "compfn;":0x2218, "complement;":0x2201,
        "complexes;":0x2102, "cong;":0x2245,
        "congdot;":0x2a6d, "conint;":0x222e,
        "copf;":[0xd835,0xdd54], "coprod;":0x2210,
        "copy;":0xa9, "copysr;":0x2117,
        "crarr;":0x21b5, "cross;":0x2717,
        "cscr;":[0xd835,0xdcb8], "csub;":0x2acf,
        "csube;":0x2ad1, "csup;":0x2ad0,
        "csupe;":0x2ad2, "ctdot;":0x22ef,
        "cudarrl;":0x2938, "cudarrr;":0x2935,
        "cuepr;":0x22de, "cuesc;":0x22df,
        "cularr;":0x21b6, "cularrp;":0x293d,
        "cup;":0x222a, "cupbrcap;":0x2a48,
        "cupcap;":0x2a46, "cupcup;":0x2a4a,
        "cupdot;":0x228d, "cupor;":0x2a45,
        "cups;":[0x222a,0xfe00], "curarr;":0x21b7,
        "curarrm;":0x293c, "curlyeqprec;":0x22de,
        "curlyeqsucc;":0x22df, "curlyvee;":0x22ce,
        "curlywedge;":0x22cf, "curren;":0xa4,
        "curvearrowleft;":0x21b6, "curvearrowright;":0x21b7,
        "cuvee;":0x22ce, "cuwed;":0x22cf,
        "cwconint;":0x2232, "cwint;":0x2231,
        "cylcty;":0x232d, "dArr;":0x21d3,
        "dHar;":0x2965, "dagger;":0x2020,
        "daleth;":0x2138, "darr;":0x2193,
        "dash;":0x2010, "dashv;":0x22a3,
        "dbkarow;":0x290f, "dblac;":0x2dd,
        "dcaron;":0x10f, "dcy;":0x434,
        "dd;":0x2146, "ddagger;":0x2021,
        "ddarr;":0x21ca, "ddotseq;":0x2a77,
        "deg;":0xb0, "delta;":0x3b4,
        "demptyv;":0x29b1, "dfisht;":0x297f,
        "dfr;":[0xd835,0xdd21], "dharl;":0x21c3,
        "dharr;":0x21c2, "diam;":0x22c4,
        "diamond;":0x22c4, "diamondsuit;":0x2666,
        "diams;":0x2666, "die;":0xa8,
        "digamma;":0x3dd, "disin;":0x22f2,
        "div;":0xf7, "divide;":0xf7,
        "divideontimes;":0x22c7, "divonx;":0x22c7,
        "djcy;":0x452, "dlcorn;":0x231e,
        "dlcrop;":0x230d, "dollar;":0x24,
        "dopf;":[0xd835,0xdd55], "dot;":0x2d9,
        "doteq;":0x2250, "doteqdot;":0x2251,
        "dotminus;":0x2238, "dotplus;":0x2214,
        "dotsquare;":0x22a1, "doublebarwedge;":0x2306,
        "downarrow;":0x2193, "downdownarrows;":0x21ca,
        "downharpoonleft;":0x21c3, "downharpoonright;":0x21c2,
        "drbkarow;":0x2910, "drcorn;":0x231f,
        "drcrop;":0x230c, "dscr;":[0xd835,0xdcb9],
        "dscy;":0x455, "dsol;":0x29f6,
        "dstrok;":0x111, "dtdot;":0x22f1,
        "dtri;":0x25bf, "dtrif;":0x25be,
        "duarr;":0x21f5, "duhar;":0x296f,
        "dwangle;":0x29a6, "dzcy;":0x45f,
        "dzigrarr;":0x27ff, "eDDot;":0x2a77,
        "eDot;":0x2251, "eacute;":0xe9,
        "easter;":0x2a6e, "ecaron;":0x11b,
        "ecir;":0x2256, "ecirc;":0xea,
        "ecolon;":0x2255, "ecy;":0x44d,
        "edot;":0x117, "ee;":0x2147,
        "efDot;":0x2252, "efr;":[0xd835,0xdd22],
        "eg;":0x2a9a, "egrave;":0xe8,
        "egs;":0x2a96, "egsdot;":0x2a98,
        "el;":0x2a99, "elinters;":0x23e7,
        "ell;":0x2113, "els;":0x2a95,
        "elsdot;":0x2a97, "emacr;":0x113,
        "empty;":0x2205, "emptyset;":0x2205,
        "emptyv;":0x2205, "emsp;":0x2003,
        "emsp13;":0x2004, "emsp14;":0x2005,
        "eng;":0x14b, "ensp;":0x2002,
        "eogon;":0x119, "eopf;":[0xd835,0xdd56],
        "epar;":0x22d5, "eparsl;":0x29e3,
        "eplus;":0x2a71, "epsi;":0x3b5,
        "epsilon;":0x3b5, "epsiv;":0x3f5,
        "eqcirc;":0x2256, "eqcolon;":0x2255,
        "eqsim;":0x2242, "eqslantgtr;":0x2a96,
        "eqslantless;":0x2a95, "equals;":0x3d,
        "equest;":0x225f, "equiv;":0x2261,
        "equivDD;":0x2a78, "eqvparsl;":0x29e5,
        "erDot;":0x2253, "erarr;":0x2971,
        "escr;":0x212f, "esdot;":0x2250,
        "esim;":0x2242, "eta;":0x3b7,
        "eth;":0xf0, "euml;":0xeb,
        "euro;":0x20ac, "excl;":0x21,
        "exist;":0x2203, "expectation;":0x2130,
        "exponentiale;":0x2147, "fallingdotseq;":0x2252,
        "fcy;":0x444, "female;":0x2640,
        "ffilig;":0xfb03, "fflig;":0xfb00,
        "ffllig;":0xfb04, "ffr;":[0xd835,0xdd23],
        "filig;":0xfb01, "fjlig;":[0x66,0x6a],
        "flat;":0x266d, "fllig;":0xfb02,
        "fltns;":0x25b1, "fnof;":0x192,
        "fopf;":[0xd835,0xdd57], "forall;":0x2200,
        "fork;":0x22d4, "forkv;":0x2ad9,
        "fpartint;":0x2a0d, "frac12;":0xbd,
        "frac13;":0x2153, "frac14;":0xbc,
        "frac15;":0x2155, "frac16;":0x2159,
        "frac18;":0x215b, "frac23;":0x2154,
        "frac25;":0x2156, "frac34;":0xbe,
        "frac35;":0x2157, "frac38;":0x215c,
        "frac45;":0x2158, "frac56;":0x215a,
        "frac58;":0x215d, "frac78;":0x215e,
        "frasl;":0x2044, "frown;":0x2322,
        "fscr;":[0xd835,0xdcbb], "gE;":0x2267,
        "gEl;":0x2a8c, "gacute;":0x1f5,
        "gamma;":0x3b3, "gammad;":0x3dd,
        "gap;":0x2a86, "gbreve;":0x11f,
        "gcirc;":0x11d, "gcy;":0x433,
        "gdot;":0x121, "ge;":0x2265,
        "gel;":0x22db, "geq;":0x2265,
        "geqq;":0x2267, "geqslant;":0x2a7e,
        "ges;":0x2a7e, "gescc;":0x2aa9,
        "gesdot;":0x2a80, "gesdoto;":0x2a82,
        "gesdotol;":0x2a84, "gesl;":[0x22db,0xfe00],
        "gesles;":0x2a94, "gfr;":[0xd835,0xdd24],
        "gg;":0x226b, "ggg;":0x22d9,
        "gimel;":0x2137, "gjcy;":0x453,
        "gl;":0x2277, "glE;":0x2a92,
        "gla;":0x2aa5, "glj;":0x2aa4,
        "gnE;":0x2269, "gnap;":0x2a8a,
        "gnapprox;":0x2a8a, "gne;":0x2a88,
        "gneq;":0x2a88, "gneqq;":0x2269,
        "gnsim;":0x22e7, "gopf;":[0xd835,0xdd58],
        "grave;":0x60, "gscr;":0x210a,
        "gsim;":0x2273, "gsime;":0x2a8e,
        "gsiml;":0x2a90, "gt;":0x3e,
        "gtcc;":0x2aa7, "gtcir;":0x2a7a,
        "gtdot;":0x22d7, "gtlPar;":0x2995,
        "gtquest;":0x2a7c, "gtrapprox;":0x2a86,
        "gtrarr;":0x2978, "gtrdot;":0x22d7,
        "gtreqless;":0x22db, "gtreqqless;":0x2a8c,
        "gtrless;":0x2277, "gtrsim;":0x2273,
        "gvertneqq;":[0x2269,0xfe00], "gvnE;":[0x2269,0xfe00],
        "hArr;":0x21d4, "hairsp;":0x200a,
        "half;":0xbd, "hamilt;":0x210b,
        "hardcy;":0x44a, "harr;":0x2194,
        "harrcir;":0x2948, "harrw;":0x21ad,
        "hbar;":0x210f, "hcirc;":0x125,
        "hearts;":0x2665, "heartsuit;":0x2665,
        "hellip;":0x2026, "hercon;":0x22b9,
        "hfr;":[0xd835,0xdd25], "hksearow;":0x2925,
        "hkswarow;":0x2926, "hoarr;":0x21ff,
        "homtht;":0x223b, "hookleftarrow;":0x21a9,
        "hookrightarrow;":0x21aa, "hopf;":[0xd835,0xdd59],
        "horbar;":0x2015, "hscr;":[0xd835,0xdcbd],
        "hslash;":0x210f, "hstrok;":0x127,
        "hybull;":0x2043, "hyphen;":0x2010,
        "iacute;":0xed, "ic;":0x2063,
        "icirc;":0xee, "icy;":0x438,
        "iecy;":0x435, "iexcl;":0xa1,
        "iff;":0x21d4, "ifr;":[0xd835,0xdd26],
        "igrave;":0xec, "ii;":0x2148,
        "iiiint;":0x2a0c, "iiint;":0x222d,
        "iinfin;":0x29dc, "iiota;":0x2129,
        "ijlig;":0x133, "imacr;":0x12b,
        "image;":0x2111, "imagline;":0x2110,
        "imagpart;":0x2111, "imath;":0x131,
        "imof;":0x22b7, "imped;":0x1b5,
        "in;":0x2208, "incare;":0x2105,
        "infin;":0x221e, "infintie;":0x29dd,
        "inodot;":0x131, "int;":0x222b,
        "intcal;":0x22ba, "integers;":0x2124,
        "intercal;":0x22ba, "intlarhk;":0x2a17,
        "intprod;":0x2a3c, "iocy;":0x451,
        "iogon;":0x12f, "iopf;":[0xd835,0xdd5a],
        "iota;":0x3b9, "iprod;":0x2a3c,
        "iquest;":0xbf, "iscr;":[0xd835,0xdcbe],
        "isin;":0x2208, "isinE;":0x22f9,
        "isindot;":0x22f5, "isins;":0x22f4,
        "isinsv;":0x22f3, "isinv;":0x2208,
        "it;":0x2062, "itilde;":0x129,
        "iukcy;":0x456, "iuml;":0xef,
        "jcirc;":0x135, "jcy;":0x439,
        "jfr;":[0xd835,0xdd27], "jmath;":0x237,
        "jopf;":[0xd835,0xdd5b], "jscr;":[0xd835,0xdcbf],
        "jsercy;":0x458, "jukcy;":0x454,
        "kappa;":0x3ba, "kappav;":0x3f0,
        "kcedil;":0x137, "kcy;":0x43a,
        "kfr;":[0xd835,0xdd28], "kgreen;":0x138,
        "khcy;":0x445, "kjcy;":0x45c,
        "kopf;":[0xd835,0xdd5c], "kscr;":[0xd835,0xdcc0],
        "lAarr;":0x21da, "lArr;":0x21d0,
        "lAtail;":0x291b, "lBarr;":0x290e,
        "lE;":0x2266, "lEg;":0x2a8b,
        "lHar;":0x2962, "lacute;":0x13a,
        "laemptyv;":0x29b4, "lagran;":0x2112,
        "lambda;":0x3bb, "lang;":0x27e8,
        "langd;":0x2991, "langle;":0x27e8,
        "lap;":0x2a85, "laquo;":0xab,
        "larr;":0x2190, "larrb;":0x21e4,
        "larrbfs;":0x291f, "larrfs;":0x291d,
        "larrhk;":0x21a9, "larrlp;":0x21ab,
        "larrpl;":0x2939, "larrsim;":0x2973,
        "larrtl;":0x21a2, "lat;":0x2aab,
        "latail;":0x2919, "late;":0x2aad,
        "lates;":[0x2aad,0xfe00], "lbarr;":0x290c,
        "lbbrk;":0x2772, "lbrace;":0x7b,
        "lbrack;":0x5b, "lbrke;":0x298b,
        "lbrksld;":0x298f, "lbrkslu;":0x298d,
        "lcaron;":0x13e, "lcedil;":0x13c,
        "lceil;":0x2308, "lcub;":0x7b,
        "lcy;":0x43b, "ldca;":0x2936,
        "ldquo;":0x201c, "ldquor;":0x201e,
        "ldrdhar;":0x2967, "ldrushar;":0x294b,
        "ldsh;":0x21b2, "le;":0x2264,
        "leftarrow;":0x2190, "leftarrowtail;":0x21a2,
        "leftharpoondown;":0x21bd, "leftharpoonup;":0x21bc,
        "leftleftarrows;":0x21c7, "leftrightarrow;":0x2194,
        "leftrightarrows;":0x21c6, "leftrightharpoons;":0x21cb,
        "leftrightsquigarrow;":0x21ad, "leftthreetimes;":0x22cb,
        "leg;":0x22da, "leq;":0x2264,
        "leqq;":0x2266, "leqslant;":0x2a7d,
        "les;":0x2a7d, "lescc;":0x2aa8,
        "lesdot;":0x2a7f, "lesdoto;":0x2a81,
        "lesdotor;":0x2a83, "lesg;":[0x22da,0xfe00],
        "lesges;":0x2a93, "lessapprox;":0x2a85,
        "lessdot;":0x22d6, "lesseqgtr;":0x22da,
        "lesseqqgtr;":0x2a8b, "lessgtr;":0x2276,
        "lesssim;":0x2272, "lfisht;":0x297c,
        "lfloor;":0x230a, "lfr;":[0xd835,0xdd29],
        "lg;":0x2276, "lgE;":0x2a91,
        "lhard;":0x21bd, "lharu;":0x21bc,
        "lharul;":0x296a, "lhblk;":0x2584,
        "ljcy;":0x459, "ll;":0x226a,
        "llarr;":0x21c7, "llcorner;":0x231e,
        "llhard;":0x296b, "lltri;":0x25fa,
        "lmidot;":0x140, "lmoust;":0x23b0,
        "lmoustache;":0x23b0, "lnE;":0x2268,
        "lnap;":0x2a89, "lnapprox;":0x2a89,
        "lne;":0x2a87, "lneq;":0x2a87,
        "lneqq;":0x2268, "lnsim;":0x22e6,
        "loang;":0x27ec, "loarr;":0x21fd,
        "lobrk;":0x27e6, "longleftarrow;":0x27f5,
        "longleftrightarrow;":0x27f7, "longmapsto;":0x27fc,
        "longrightarrow;":0x27f6, "looparrowleft;":0x21ab,
        "looparrowright;":0x21ac, "lopar;":0x2985,
        "lopf;":[0xd835,0xdd5d], "loplus;":0x2a2d,
        "lotimes;":0x2a34, "lowast;":0x2217,
        "lowbar;":0x5f, "loz;":0x25ca,
        "lozenge;":0x25ca, "lozf;":0x29eb,
        "lpar;":0x28, "lparlt;":0x2993,
        "lrarr;":0x21c6, "lrcorner;":0x231f,
        "lrhar;":0x21cb, "lrhard;":0x296d,
        "lrm;":0x200e, "lrtri;":0x22bf,
        "lsaquo;":0x2039, "lscr;":[0xd835,0xdcc1],
        "lsh;":0x21b0, "lsim;":0x2272,
        "lsime;":0x2a8d, "lsimg;":0x2a8f,
        "lsqb;":0x5b, "lsquo;":0x2018,
        "lsquor;":0x201a, "lstrok;":0x142,
        "lt;":0x3c, "ltcc;":0x2aa6,
        "ltcir;":0x2a79, "ltdot;":0x22d6,
        "lthree;":0x22cb, "ltimes;":0x22c9,
        "ltlarr;":0x2976, "ltquest;":0x2a7b,
        "ltrPar;":0x2996, "ltri;":0x25c3,
        "ltrie;":0x22b4, "ltrif;":0x25c2,
        "lurdshar;":0x294a, "luruhar;":0x2966,
        "lvertneqq;":[0x2268,0xfe00], "lvnE;":[0x2268,0xfe00],
        "mDDot;":0x223a, "macr;":0xaf,
        "male;":0x2642, "malt;":0x2720,
        "maltese;":0x2720, "map;":0x21a6,
        "mapsto;":0x21a6, "mapstodown;":0x21a7,
        "mapstoleft;":0x21a4, "mapstoup;":0x21a5,
        "marker;":0x25ae, "mcomma;":0x2a29,
        "mcy;":0x43c, "mdash;":0x2014,
        "measuredangle;":0x2221, "mfr;":[0xd835,0xdd2a],
        "mho;":0x2127, "micro;":0xb5,
        "mid;":0x2223, "midast;":0x2a,
        "midcir;":0x2af0, "middot;":0xb7,
        "minus;":0x2212, "minusb;":0x229f,
        "minusd;":0x2238, "minusdu;":0x2a2a,
        "mlcp;":0x2adb, "mldr;":0x2026,
        "mnplus;":0x2213, "models;":0x22a7,
        "mopf;":[0xd835,0xdd5e], "mp;":0x2213,
        "mscr;":[0xd835,0xdcc2], "mstpos;":0x223e,
        "mu;":0x3bc, "multimap;":0x22b8,
        "mumap;":0x22b8, "nGg;":[0x22d9,0x338],
        "nGt;":[0x226b,0x20d2], "nGtv;":[0x226b,0x338],
        "nLeftarrow;":0x21cd, "nLeftrightarrow;":0x21ce,
        "nLl;":[0x22d8,0x338], "nLt;":[0x226a,0x20d2],
        "nLtv;":[0x226a,0x338], "nRightarrow;":0x21cf,
        "nVDash;":0x22af, "nVdash;":0x22ae,
        "nabla;":0x2207, "nacute;":0x144,
        "nang;":[0x2220,0x20d2], "nap;":0x2249,
        "napE;":[0x2a70,0x338], "napid;":[0x224b,0x338],
        "napos;":0x149, "napprox;":0x2249,
        "natur;":0x266e, "natural;":0x266e,
        "naturals;":0x2115, "nbsp;":0xa0,
        "nbump;":[0x224e,0x338], "nbumpe;":[0x224f,0x338],
        "ncap;":0x2a43, "ncaron;":0x148,
        "ncedil;":0x146, "ncong;":0x2247,
        "ncongdot;":[0x2a6d,0x338], "ncup;":0x2a42,
        "ncy;":0x43d, "ndash;":0x2013,
        "ne;":0x2260, "neArr;":0x21d7,
        "nearhk;":0x2924, "nearr;":0x2197,
        "nearrow;":0x2197, "nedot;":[0x2250,0x338],
        "nequiv;":0x2262, "nesear;":0x2928,
        "nesim;":[0x2242,0x338], "nexist;":0x2204,
        "nexists;":0x2204, "nfr;":[0xd835,0xdd2b],
        "ngE;":[0x2267,0x338], "nge;":0x2271,
        "ngeq;":0x2271, "ngeqq;":[0x2267,0x338],
        "ngeqslant;":[0x2a7e,0x338], "nges;":[0x2a7e,0x338],
        "ngsim;":0x2275, "ngt;":0x226f,
        "ngtr;":0x226f, "nhArr;":0x21ce,
        "nharr;":0x21ae, "nhpar;":0x2af2,
        "ni;":0x220b, "nis;":0x22fc,
        "nisd;":0x22fa, "niv;":0x220b,
        "njcy;":0x45a, "nlArr;":0x21cd,
        "nlE;":[0x2266,0x338], "nlarr;":0x219a,
        "nldr;":0x2025, "nle;":0x2270,
        "nleftarrow;":0x219a, "nleftrightarrow;":0x21ae,
        "nleq;":0x2270, "nleqq;":[0x2266,0x338],
        "nleqslant;":[0x2a7d,0x338], "nles;":[0x2a7d,0x338],
        "nless;":0x226e, "nlsim;":0x2274,
        "nlt;":0x226e, "nltri;":0x22ea,
        "nltrie;":0x22ec, "nmid;":0x2224,
        "nopf;":[0xd835,0xdd5f], "not;":0xac,
        "notin;":0x2209, "notinE;":[0x22f9,0x338],
        "notindot;":[0x22f5,0x338], "notinva;":0x2209,
        "notinvb;":0x22f7, "notinvc;":0x22f6,
        "notni;":0x220c, "notniva;":0x220c,
        "notnivb;":0x22fe, "notnivc;":0x22fd,
        "npar;":0x2226, "nparallel;":0x2226,
        "nparsl;":[0x2afd,0x20e5], "npart;":[0x2202,0x338],
        "npolint;":0x2a14, "npr;":0x2280,
        "nprcue;":0x22e0, "npre;":[0x2aaf,0x338],
        "nprec;":0x2280, "npreceq;":[0x2aaf,0x338],
        "nrArr;":0x21cf, "nrarr;":0x219b,
        "nrarrc;":[0x2933,0x338], "nrarrw;":[0x219d,0x338],
        "nrightarrow;":0x219b, "nrtri;":0x22eb,
        "nrtrie;":0x22ed, "nsc;":0x2281,
        "nsccue;":0x22e1, "nsce;":[0x2ab0,0x338],
        "nscr;":[0xd835,0xdcc3], "nshortmid;":0x2224,
        "nshortparallel;":0x2226, "nsim;":0x2241,
        "nsime;":0x2244, "nsimeq;":0x2244,
        "nsmid;":0x2224, "nspar;":0x2226,
        "nsqsube;":0x22e2, "nsqsupe;":0x22e3,
        "nsub;":0x2284, "nsubE;":[0x2ac5,0x338],
        "nsube;":0x2288, "nsubset;":[0x2282,0x20d2],
        "nsubseteq;":0x2288, "nsubseteqq;":[0x2ac5,0x338],
        "nsucc;":0x2281, "nsucceq;":[0x2ab0,0x338],
        "nsup;":0x2285, "nsupE;":[0x2ac6,0x338],
        "nsupe;":0x2289, "nsupset;":[0x2283,0x20d2],
        "nsupseteq;":0x2289, "nsupseteqq;":[0x2ac6,0x338],
        "ntgl;":0x2279, "ntilde;":0xf1,
        "ntlg;":0x2278, "ntriangleleft;":0x22ea,
        "ntrianglelefteq;":0x22ec, "ntriangleright;":0x22eb,
        "ntrianglerighteq;":0x22ed, "nu;":0x3bd,
        "num;":0x23, "numero;":0x2116,
        "numsp;":0x2007, "nvDash;":0x22ad,
        "nvHarr;":0x2904, "nvap;":[0x224d,0x20d2],
        "nvdash;":0x22ac, "nvge;":[0x2265,0x20d2],
        "nvgt;":[0x3e,0x20d2], "nvinfin;":0x29de,
        "nvlArr;":0x2902, "nvle;":[0x2264,0x20d2],
        "nvlt;":[0x3c,0x20d2], "nvltrie;":[0x22b4,0x20d2],
        "nvrArr;":0x2903, "nvrtrie;":[0x22b5,0x20d2],
        "nvsim;":[0x223c,0x20d2], "nwArr;":0x21d6,
        "nwarhk;":0x2923, "nwarr;":0x2196,
        "nwarrow;":0x2196, "nwnear;":0x2927,
        "oS;":0x24c8, "oacute;":0xf3,
        "oast;":0x229b, "ocir;":0x229a,
        "ocirc;":0xf4, "ocy;":0x43e,
        "odash;":0x229d, "odblac;":0x151,
        "odiv;":0x2a38, "odot;":0x2299,
        "odsold;":0x29bc, "oelig;":0x153,
        "ofcir;":0x29bf, "ofr;":[0xd835,0xdd2c],
        "ogon;":0x2db, "ograve;":0xf2,
        "ogt;":0x29c1, "ohbar;":0x29b5,
        "ohm;":0x3a9, "oint;":0x222e,
        "olarr;":0x21ba, "olcir;":0x29be,
        "olcross;":0x29bb, "oline;":0x203e,
        "olt;":0x29c0, "omacr;":0x14d,
        "omega;":0x3c9, "omicron;":0x3bf,
        "omid;":0x29b6, "ominus;":0x2296,
        "oopf;":[0xd835,0xdd60], "opar;":0x29b7,
        "operp;":0x29b9, "oplus;":0x2295,
        "or;":0x2228, "orarr;":0x21bb,
        "ord;":0x2a5d, "order;":0x2134,
        "orderof;":0x2134, "ordf;":0xaa,
        "ordm;":0xba, "origof;":0x22b6,
        "oror;":0x2a56, "orslope;":0x2a57,
        "orv;":0x2a5b, "oscr;":0x2134,
        "oslash;":0xf8, "osol;":0x2298,
        "otilde;":0xf5, "otimes;":0x2297,
        "otimesas;":0x2a36, "ouml;":0xf6,
        "ovbar;":0x233d, "par;":0x2225,
        "para;":0xb6, "parallel;":0x2225,
        "parsim;":0x2af3, "parsl;":0x2afd,
        "part;":0x2202, "pcy;":0x43f,
        "percnt;":0x25, "period;":0x2e,
        "permil;":0x2030, "perp;":0x22a5,
        "pertenk;":0x2031, "pfr;":[0xd835,0xdd2d],
        "phi;":0x3c6, "phiv;":0x3d5,
        "phmmat;":0x2133, "phone;":0x260e,
        "pi;":0x3c0, "pitchfork;":0x22d4,
        "piv;":0x3d6, "planck;":0x210f,
        "planckh;":0x210e, "plankv;":0x210f,
        "plus;":0x2b, "plusacir;":0x2a23,
        "plusb;":0x229e, "pluscir;":0x2a22,
        "plusdo;":0x2214, "plusdu;":0x2a25,
        "pluse;":0x2a72, "plusmn;":0xb1,
        "plussim;":0x2a26, "plustwo;":0x2a27,
        "pm;":0xb1, "pointint;":0x2a15,
        "popf;":[0xd835,0xdd61], "pound;":0xa3,
        "pr;":0x227a, "prE;":0x2ab3,
        "prap;":0x2ab7, "prcue;":0x227c,
        "pre;":0x2aaf, "prec;":0x227a,
        "precapprox;":0x2ab7, "preccurlyeq;":0x227c,
        "preceq;":0x2aaf, "precnapprox;":0x2ab9,
        "precneqq;":0x2ab5, "precnsim;":0x22e8,
        "precsim;":0x227e, "prime;":0x2032,
        "primes;":0x2119, "prnE;":0x2ab5,
        "prnap;":0x2ab9, "prnsim;":0x22e8,
        "prod;":0x220f, "profalar;":0x232e,
        "profline;":0x2312, "profsurf;":0x2313,
        "prop;":0x221d, "propto;":0x221d,
        "prsim;":0x227e, "prurel;":0x22b0,
        "pscr;":[0xd835,0xdcc5], "psi;":0x3c8,
        "puncsp;":0x2008, "qfr;":[0xd835,0xdd2e],
        "qint;":0x2a0c, "qopf;":[0xd835,0xdd62],
        "qprime;":0x2057, "qscr;":[0xd835,0xdcc6],
        "quaternions;":0x210d, "quatint;":0x2a16,
        "quest;":0x3f, "questeq;":0x225f,
        "quot;":0x22, "rAarr;":0x21db,
        "rArr;":0x21d2, "rAtail;":0x291c,
        "rBarr;":0x290f, "rHar;":0x2964,
        "race;":[0x223d,0x331], "racute;":0x155,
        "radic;":0x221a, "raemptyv;":0x29b3,
        "rang;":0x27e9, "rangd;":0x2992,
        "range;":0x29a5, "rangle;":0x27e9,
        "raquo;":0xbb, "rarr;":0x2192,
        "rarrap;":0x2975, "rarrb;":0x21e5,
        "rarrbfs;":0x2920, "rarrc;":0x2933,
        "rarrfs;":0x291e, "rarrhk;":0x21aa,
        "rarrlp;":0x21ac, "rarrpl;":0x2945,
        "rarrsim;":0x2974, "rarrtl;":0x21a3,
        "rarrw;":0x219d, "ratail;":0x291a,
        "ratio;":0x2236, "rationals;":0x211a,
        "rbarr;":0x290d, "rbbrk;":0x2773,
        "rbrace;":0x7d, "rbrack;":0x5d,
        "rbrke;":0x298c, "rbrksld;":0x298e,
        "rbrkslu;":0x2990, "rcaron;":0x159,
        "rcedil;":0x157, "rceil;":0x2309,
        "rcub;":0x7d, "rcy;":0x440,
        "rdca;":0x2937, "rdldhar;":0x2969,
        "rdquo;":0x201d, "rdquor;":0x201d,
        "rdsh;":0x21b3, "real;":0x211c,
        "realine;":0x211b, "realpart;":0x211c,
        "reals;":0x211d, "rect;":0x25ad,
        "reg;":0xae, "rfisht;":0x297d,
        "rfloor;":0x230b, "rfr;":[0xd835,0xdd2f],
        "rhard;":0x21c1, "rharu;":0x21c0,
        "rharul;":0x296c, "rho;":0x3c1,
        "rhov;":0x3f1, "rightarrow;":0x2192,
        "rightarrowtail;":0x21a3, "rightharpoondown;":0x21c1,
        "rightharpoonup;":0x21c0, "rightleftarrows;":0x21c4,
        "rightleftharpoons;":0x21cc, "rightrightarrows;":0x21c9,
        "rightsquigarrow;":0x219d, "rightthreetimes;":0x22cc,
        "ring;":0x2da, "risingdotseq;":0x2253,
        "rlarr;":0x21c4, "rlhar;":0x21cc,
        "rlm;":0x200f, "rmoust;":0x23b1,
        "rmoustache;":0x23b1, "rnmid;":0x2aee,
        "roang;":0x27ed, "roarr;":0x21fe,
        "robrk;":0x27e7, "ropar;":0x2986,
        "ropf;":[0xd835,0xdd63], "roplus;":0x2a2e,
        "rotimes;":0x2a35, "rpar;":0x29,
        "rpargt;":0x2994, "rppolint;":0x2a12,
        "rrarr;":0x21c9, "rsaquo;":0x203a,
        "rscr;":[0xd835,0xdcc7], "rsh;":0x21b1,
        "rsqb;":0x5d, "rsquo;":0x2019,
        "rsquor;":0x2019, "rthree;":0x22cc,
        "rtimes;":0x22ca, "rtri;":0x25b9,
        "rtrie;":0x22b5, "rtrif;":0x25b8,
        "rtriltri;":0x29ce, "ruluhar;":0x2968,
        "rx;":0x211e, "sacute;":0x15b,
        "sbquo;":0x201a, "sc;":0x227b,
        "scE;":0x2ab4, "scap;":0x2ab8,
        "scaron;":0x161, "sccue;":0x227d,
        "sce;":0x2ab0, "scedil;":0x15f,
        "scirc;":0x15d, "scnE;":0x2ab6,
        "scnap;":0x2aba, "scnsim;":0x22e9,
        "scpolint;":0x2a13, "scsim;":0x227f,
        "scy;":0x441, "sdot;":0x22c5,
        "sdotb;":0x22a1, "sdote;":0x2a66,
        "seArr;":0x21d8, "searhk;":0x2925,
        "searr;":0x2198, "searrow;":0x2198,
        "sect;":0xa7, "semi;":0x3b,
        "seswar;":0x2929, "setminus;":0x2216,
        "setmn;":0x2216, "sext;":0x2736,
        "sfr;":[0xd835,0xdd30], "sfrown;":0x2322,
        "sharp;":0x266f, "shchcy;":0x449,
        "shcy;":0x448, "shortmid;":0x2223,
        "shortparallel;":0x2225, "shy;":0xad,
        "sigma;":0x3c3, "sigmaf;":0x3c2,
        "sigmav;":0x3c2, "sim;":0x223c,
        "simdot;":0x2a6a, "sime;":0x2243,
        "simeq;":0x2243, "simg;":0x2a9e,
        "simgE;":0x2aa0, "siml;":0x2a9d,
        "simlE;":0x2a9f, "simne;":0x2246,
        "simplus;":0x2a24, "simrarr;":0x2972,
        "slarr;":0x2190, "smallsetminus;":0x2216,
        "smashp;":0x2a33, "smeparsl;":0x29e4,
        "smid;":0x2223, "smile;":0x2323,
        "smt;":0x2aaa, "smte;":0x2aac,
        "smtes;":[0x2aac,0xfe00], "softcy;":0x44c,
        "sol;":0x2f, "solb;":0x29c4,
        "solbar;":0x233f, "sopf;":[0xd835,0xdd64],
        "spades;":0x2660, "spadesuit;":0x2660,
        "spar;":0x2225, "sqcap;":0x2293,
        "sqcaps;":[0x2293,0xfe00], "sqcup;":0x2294,
        "sqcups;":[0x2294,0xfe00], "sqsub;":0x228f,
        "sqsube;":0x2291, "sqsubset;":0x228f,
        "sqsubseteq;":0x2291, "sqsup;":0x2290,
        "sqsupe;":0x2292, "sqsupset;":0x2290,
        "sqsupseteq;":0x2292, "squ;":0x25a1,
        "square;":0x25a1, "squarf;":0x25aa,
        "squf;":0x25aa, "srarr;":0x2192,
        "sscr;":[0xd835,0xdcc8], "ssetmn;":0x2216,
        "ssmile;":0x2323, "sstarf;":0x22c6,
        "star;":0x2606, "starf;":0x2605,
        "straightepsilon;":0x3f5, "straightphi;":0x3d5,
        "strns;":0xaf, "sub;":0x2282,
        "subE;":0x2ac5, "subdot;":0x2abd,
        "sube;":0x2286, "subedot;":0x2ac3,
        "submult;":0x2ac1, "subnE;":0x2acb,
        "subne;":0x228a, "subplus;":0x2abf,
        "subrarr;":0x2979, "subset;":0x2282,
        "subseteq;":0x2286, "subseteqq;":0x2ac5,
        "subsetneq;":0x228a, "subsetneqq;":0x2acb,
        "subsim;":0x2ac7, "subsub;":0x2ad5,
        "subsup;":0x2ad3, "succ;":0x227b,
        "succapprox;":0x2ab8, "succcurlyeq;":0x227d,
        "succeq;":0x2ab0, "succnapprox;":0x2aba,
        "succneqq;":0x2ab6, "succnsim;":0x22e9,
        "succsim;":0x227f, "sum;":0x2211,
        "sung;":0x266a, "sup;":0x2283,
        "sup1;":0xb9, "sup2;":0xb2,
        "sup3;":0xb3, "supE;":0x2ac6,
        "supdot;":0x2abe, "supdsub;":0x2ad8,
        "supe;":0x2287, "supedot;":0x2ac4,
        "suphsol;":0x27c9, "suphsub;":0x2ad7,
        "suplarr;":0x297b, "supmult;":0x2ac2,
        "supnE;":0x2acc, "supne;":0x228b,
        "supplus;":0x2ac0, "supset;":0x2283,
        "supseteq;":0x2287, "supseteqq;":0x2ac6,
        "supsetneq;":0x228b, "supsetneqq;":0x2acc,
        "supsim;":0x2ac8, "supsub;":0x2ad4,
        "supsup;":0x2ad6, "swArr;":0x21d9,
        "swarhk;":0x2926, "swarr;":0x2199,
        "swarrow;":0x2199, "swnwar;":0x292a,
        "szlig;":0xdf, "target;":0x2316,
        "tau;":0x3c4, "tbrk;":0x23b4,
        "tcaron;":0x165, "tcedil;":0x163,
        "tcy;":0x442, "tdot;":0x20db,
        "telrec;":0x2315, "tfr;":[0xd835,0xdd31],
        "there4;":0x2234, "therefore;":0x2234,
        "theta;":0x3b8, "thetasym;":0x3d1,
        "thetav;":0x3d1, "thickapprox;":0x2248,
        "thicksim;":0x223c, "thinsp;":0x2009,
        "thkap;":0x2248, "thksim;":0x223c,
        "thorn;":0xfe, "tilde;":0x2dc,
        "times;":0xd7, "timesb;":0x22a0,
        "timesbar;":0x2a31, "timesd;":0x2a30,
        "tint;":0x222d, "toea;":0x2928,
        "top;":0x22a4, "topbot;":0x2336,
        "topcir;":0x2af1, "topf;":[0xd835,0xdd65],
        "topfork;":0x2ada, "tosa;":0x2929,
        "tprime;":0x2034, "trade;":0x2122,
        "triangle;":0x25b5, "triangledown;":0x25bf,
        "triangleleft;":0x25c3, "trianglelefteq;":0x22b4,
        "triangleq;":0x225c, "triangleright;":0x25b9,
        "trianglerighteq;":0x22b5, "tridot;":0x25ec,
        "trie;":0x225c, "triminus;":0x2a3a,
        "triplus;":0x2a39, "trisb;":0x29cd,
        "tritime;":0x2a3b, "trpezium;":0x23e2,
        "tscr;":[0xd835,0xdcc9], "tscy;":0x446,
        "tshcy;":0x45b, "tstrok;":0x167,
        "twixt;":0x226c, "twoheadleftarrow;":0x219e,
        "twoheadrightarrow;":0x21a0, "uArr;":0x21d1,
        "uHar;":0x2963, "uacute;":0xfa,
        "uarr;":0x2191, "ubrcy;":0x45e,
        "ubreve;":0x16d, "ucirc;":0xfb,
        "ucy;":0x443, "udarr;":0x21c5,
        "udblac;":0x171, "udhar;":0x296e,
        "ufisht;":0x297e, "ufr;":[0xd835,0xdd32],
        "ugrave;":0xf9, "uharl;":0x21bf,
        "uharr;":0x21be, "uhblk;":0x2580,
        "ulcorn;":0x231c, "ulcorner;":0x231c,
        "ulcrop;":0x230f, "ultri;":0x25f8,
        "umacr;":0x16b, "uml;":0xa8,
        "uogon;":0x173, "uopf;":[0xd835,0xdd66],
        "uparrow;":0x2191, "updownarrow;":0x2195,
        "upharpoonleft;":0x21bf, "upharpoonright;":0x21be,
        "uplus;":0x228e, "upsi;":0x3c5,
        "upsih;":0x3d2, "upsilon;":0x3c5,
        "upuparrows;":0x21c8, "urcorn;":0x231d,
        "urcorner;":0x231d, "urcrop;":0x230e,
        "uring;":0x16f, "urtri;":0x25f9,
        "uscr;":[0xd835,0xdcca], "utdot;":0x22f0,
        "utilde;":0x169, "utri;":0x25b5,
        "utrif;":0x25b4, "uuarr;":0x21c8,
        "uuml;":0xfc, "uwangle;":0x29a7,
        "vArr;":0x21d5, "vBar;":0x2ae8,
        "vBarv;":0x2ae9, "vDash;":0x22a8,
        "vangrt;":0x299c, "varepsilon;":0x3f5,
        "varkappa;":0x3f0, "varnothing;":0x2205,
        "varphi;":0x3d5, "varpi;":0x3d6,
        "varpropto;":0x221d, "varr;":0x2195,
        "varrho;":0x3f1, "varsigma;":0x3c2,
        "varsubsetneq;":[0x228a,0xfe00], "varsubsetneqq;":[0x2acb,0xfe00],
        "varsupsetneq;":[0x228b,0xfe00], "varsupsetneqq;":[0x2acc,0xfe00],
        "vartheta;":0x3d1, "vartriangleleft;":0x22b2,
        "vartriangleright;":0x22b3, "vcy;":0x432,
        "vdash;":0x22a2, "vee;":0x2228,
        "veebar;":0x22bb, "veeeq;":0x225a,
        "vellip;":0x22ee, "verbar;":0x7c,
        "vert;":0x7c, "vfr;":[0xd835,0xdd33],
        "vltri;":0x22b2, "vnsub;":[0x2282,0x20d2],
        "vnsup;":[0x2283,0x20d2], "vopf;":[0xd835,0xdd67],
        "vprop;":0x221d, "vrtri;":0x22b3,
        "vscr;":[0xd835,0xdccb], "vsubnE;":[0x2acb,0xfe00],
        "vsubne;":[0x228a,0xfe00], "vsupnE;":[0x2acc,0xfe00],
        "vsupne;":[0x228b,0xfe00], "vzigzag;":0x299a,
        "wcirc;":0x175, "wedbar;":0x2a5f,
        "wedge;":0x2227, "wedgeq;":0x2259,
        "weierp;":0x2118, "wfr;":[0xd835,0xdd34],
        "wopf;":[0xd835,0xdd68], "wp;":0x2118,
        "wr;":0x2240, "wreath;":0x2240,
        "wscr;":[0xd835,0xdccc], "xcap;":0x22c2,
        "xcirc;":0x25ef, "xcup;":0x22c3,
        "xdtri;":0x25bd, "xfr;":[0xd835,0xdd35],
        "xhArr;":0x27fa, "xharr;":0x27f7,
        "xi;":0x3be, "xlArr;":0x27f8,
        "xlarr;":0x27f5, "xmap;":0x27fc,
        "xnis;":0x22fb, "xodot;":0x2a00,
        "xopf;":[0xd835,0xdd69], "xoplus;":0x2a01,
        "xotime;":0x2a02, "xrArr;":0x27f9,
        "xrarr;":0x27f6, "xscr;":[0xd835,0xdccd],
        "xsqcup;":0x2a06, "xuplus;":0x2a04,
        "xutri;":0x25b3, "xvee;":0x22c1,
        "xwedge;":0x22c0, "yacute;":0xfd,
        "yacy;":0x44f, "ycirc;":0x177,
        "ycy;":0x44b, "yen;":0xa5,
        "yfr;":[0xd835,0xdd36], "yicy;":0x457,
        "yopf;":[0xd835,0xdd6a], "yscr;":[0xd835,0xdcce],
        "yucy;":0x44e, "yuml;":0xff,
        "zacute;":0x17a, "zcaron;":0x17e,
        "zcy;":0x437, "zdot;":0x17c,
        "zeetrf;":0x2128, "zeta;":0x3b6,
        "zfr;":[0xd835,0xdd37], "zhcy;":0x436,
        "zigrarr;":0x21dd, "zopf;":[0xd835,0xdd6b],
        "zscr;":[0xd835,0xdccf], "zwj;":0x200d,
        "zwnj;":0x200c
    });

    // Regular expression constants used by the tokenizer and parser

    // This regular expression matches the portion of a character reference
    // (decimal, hex, or named) that comes after the ampersand. I'd like to
    // use the y modifier to make it match at lastIndex, but for compatability
    // with Node, I can't.
    const CHARREF = /^#[0-9]+[^0-9]|^#[xX][0-9a-fA-F]+[^0-9a-fA-F]|^[a-zA-Z][a-zA-Z0-9]*[^a-zA-Z0-9]/;

    // Like the above, but for named char refs, the last char can't be =
    const ATTRCHARREF = /^#[0-9]+[^0-9]|^#[xX][0-9a-fA-F]+[^0-9a-fA-F]|^[a-zA-Z][a-zA-Z0-9]*[^=a-zA-Z0-9]/;

    const DATATEXT = /[^&<\r\u0000\uffff]*/g;
    const RAWTEXT = /[^<\r\u0000\uffff]*/g;
    const PLAINTEXT = /[^\r\u0000\uffff]*/g;
    const SIMPLETAG = /^(\/)?([a-z]+)>/;
    const SIMPLEATTR = /^([a-z]+) *= *('[^'&\r\u0000]*'|"[^"&\r\u0000]*"|[^&> \t\n\r\f\u0000][ \t\n\f])/;

    const NONWS = /[^\x09\x0A\x0C\x0D\x20]/;
    const ALLNONWS = /[^\x09\x0A\x0C\x0D\x20]/g;  // like above, with g flag
    const NONWSNONNUL = /[^\x00\x09\x0A\x0C\x0D\x20]/; // don't allow NUL either
    const LEADINGWS = /^[\x09\x0A\x0C\x0D\x20]+/;
    const NULCHARS = /\x00/g;

    /***
     * These are utility functions that don't use any of the parser's
     * internal state.
     */
    function buf2str(buf) { return apply(fromCharCode,String,buf); }

    // Determine whether the element is a member of the set.
    // The set is an object that maps namespaces to objects. The objects
    // then map local tagnames to the value true if that tag is part of the set
    function isA(elt, set) {
        var tagnames = set[elt.namespaceURI];
        return tagnames ? elt.localName in tagnames : false;
    }

    function isMathmlTextIntegrationPoint(n) {
        return isA(n, mathmlTextIntegrationPointSet);
    }

    function isHTMLIntegrationPoint(n) {
        if (isA(n, htmlIntegrationPointSet)) return true;
        if (n.namespaceURI === MATHML_NAMESPACE &&
            n.localName === "annotation-xml") {
            var encoding = n.getAttribute("encoding");
            if (encoding) encoding = toLowerCase(encoding);
            if (encoding === "text/html" ||
                encoding === "application/xhtml+xml")
                return true;
        }
        return false;
    }

    function adjustSVGTagName(name) {
        if (name in svgTagNameAdjustments)
            return svgTagNameAdjustments[name];
        else
            return name;
    }

    function adjustSVGAttributes(attrs) {
        for(var i = 0, n = attrs.length; i < n; i++) {
            if (attrs[i][0] in svgAttrAdjustments) {
                attrs[i][0] = svgAttrAdjustments[attrs[i][0]];
            }
        }
    }

    function adjustMathMLAttributes(attrs) {
        for(var i = 0, n = attrs.length; i < n; i++) {
            if (attrs[i][0] === "definitionurl") {
                attrs[i][0] = "definitionURL";
                break;
            }
        }
    }

    function adjustForeignAttributes(attrs) {
        for(var i = 0, n = attrs.length; i < n; i++) {
            if (attrs[i][0] in foreignAttributes) {
                // Attributes with namespaces get a 3rd element:
                // [Qname, value, namespace]
                push(attrs[i], foreignAttributes[attrs[i][0]]);
            }
        }
    }

    // For each attribute in attrs, if elt doesn't have an attribute
    // by that name, add the attribute to elt
    // XXX: I'm ignoring namespaces for now
    function transferAttributes(attrs, elt) {
        for(var i = 0, n = attrs.length; i < n; i++) {
            var name = attrs[i][0], value = attrs[i][1];
            if (elt.hasAttribute(name)) continue;
            elt._setAttribute(name, value);
        }
    }

    /***
     * The ElementStack class
     */
    HTMLParser.ElementStack = function ElementStack() {
        this.elements = [];
        this.top = null;  // stack.top is the "current node" in the spec
    }

    /*
    // This is for debugging only
    HTMLParser.ElementStack.prototype.toString = function(e) {
        return "STACK: " +
        this.elements.map(function(e) {return e.localName;}).join("-");
    }
    */

    HTMLParser.ElementStack.prototype.push = function(e) {
        push(this.elements, e);
        this.top = e;
    };

    HTMLParser.ElementStack.prototype.pop = function(e) {
        pop(this.elements);
        this.top = this.elements[this.elements.length-1];
    };

    // Pop elements off the stack up to and including the first
    // element with the specified (HTML) tagname
    HTMLParser.ElementStack.prototype.popTag = function(tag) {
        for(var i = this.elements.length-1; i >= 0; i--) {
            var e = this.elements[i];
            if (e.namespaceURI !== HTML_NAMESPACE) continue;
            if (e.localName === tag) break;
        }
        this.elements.length = i;
        this.top = this.elements[i-1];
    };

    // Pop elements off the stack up to and including the first
    // element that is an instance of the specified type
    HTMLParser.ElementStack.prototype.popElementType = function(type) {
        for(var i = this.elements.length-1; i >= 0; i--) {
            if (this.elements[i] instanceof type) break;
        }
        this.elements.length = i;
        this.top = this.elements[i-1];
    };

    // Pop elements off the stack up to and including the element e.
    // Note that this is very different from removeElement()
    // This requires that e is on the stack.
    HTMLParser.ElementStack.prototype.popElement = function(e) {
        for(var i = this.elements.length-1; i >= 0; i--) {
            if (this.elements[i] === e) break;
        }
        this.elements.length = i;
        this.top = this.elements[i-1];
    };

    // Remove a specific element from the stack.
    // Do nothing if the element is not on the stack
    HTMLParser.ElementStack.prototype.removeElement = function(e) {
        if (this.top === e) this.pop();
        else {
            var idx = A.lastIndexOf(this.elements, e);
            if (idx !== -1)
                splice(this.elements, idx, 1);
        }
    };

    HTMLParser.ElementStack.prototype.clearToContext = function(type) {
        // Note that we don't loop to 0. Never pop the <html> elt off.
        for(var i = this.elements.length-1; i > 0; i--) {
            if (this.elements[i] instanceof type) break;
        }
        this.elements.length = i+1;
        this.top = this.elements[i];
    };

    HTMLParser.ElementStack.prototype.inSpecificScope = function(tag, set) {
        for(var i = this.elements.length-1; i >= 0; i--) {
            var elt = this.elements[i];
            var ns = elt.namespaceURI;
            var localname = elt.localName;
            if (ns === HTML_NAMESPACE && localname === tag) return true;
            var tags = set[ns];
            if (tags && localname in tags) return false;
        }
        return false;
    };

    // Like the above, but for a specific element, not a tagname
    HTMLParser.ElementStack.prototype.elementInSpecificScope = function(target, set) {
        for(var i = this.elements.length-1; i >= 0; i--) {
            var elt = this.elements[i];
            if (elt === target) return true;
            var tags = set[elt.namespaceURI];
            if (tags && elt.localName in tags) return false;
        }
        return false;
    };

    // Like the above, but for an element interface, not a tagname
    HTMLParser.ElementStack.prototype.elementTypeInSpecificScope = function(target,
                                                                 set)
    {
        for(var i = this.elements.length-1; i >= 0; i--) {
            var elt = this.elements[i];
            if (elt instanceof target) return true;
            var tags = set[elt.namespaceURI];
            if (tags && elt.localName in tags) return false;
        }
        return false;
    };

    HTMLParser.ElementStack.prototype.inScope = function(tag) {
        return this.inSpecificScope(tag, inScopeSet);
    };

    HTMLParser.ElementStack.prototype.elementInScope = function(e) {
        return this.elementInSpecificScope(e, inScopeSet);
    };

    HTMLParser.ElementStack.prototype.elementTypeInScope = function(type) {
        return this.elementTypeInSpecificScope(type, inScopeSet);
    };

    HTMLParser.ElementStack.prototype.inButtonScope = function(tag) {
        return this.inSpecificScope(tag, inButtonScopeSet);
    };

    HTMLParser.ElementStack.prototype.inListItemScope = function(tag) {
        return this.inSpecificScope(tag, inListItemScopeSet);
    };

    HTMLParser.ElementStack.prototype.inTableScope = function(tag) {
        return this.inSpecificScope(tag, inTableScopeSet);
    };

    HTMLParser.ElementStack.prototype.inSelectScope = function(tag) {
        // Can't implement this one with inSpecificScope, since it involves
        // a set defined by inverting another set. So implement manually.
        for(var i = this.elements.length-1; i >= 0; i--) {
            var elt = this.elements[i];
            if (elt.namespaceURI !== HTML_NAMESPACE) return false;
            var localname = elt.localName;
            if (localname === tag) return true;
            if (localname !== "optgroup" && localname !== "option")
                return false;
        }
        return false;
    };

    HTMLParser.ElementStack.prototype.generateImpliedEndTags = function(butnot) {
        for(var i = this.elements.length-1; i >= 0; i--) {
            var e = this.elements[i];
            if (butnot && e.localName === butnot) break;
            if (!isA(this.elements[i], impliedEndTagsSet)) break;
        }

        this.elements.length = i+1;
        this.top = this.elements[i];
    };

    /***
     * The ActiveFormattingElements class
     */
    HTMLParser.ActiveFormattingElements = function AFE() {
        this.list = [];    // elements
        this.attrs = [];   // attribute tokens for cloning
    };

    HTMLParser.ActiveFormattingElements.prototype.MARKER = { localName: "|" };

    /*
    // For debugging
    HTMLParser.ActiveFormattingElements.prototype.toString = function() {
        return "AFE: " +
        this.list.map(function(e) { return e.localName; }).join("-");
    }
    */

    HTMLParser.ActiveFormattingElements.prototype.insertMarker = function() {
        push(this.list, this.MARKER);
        push(this.attrs, this.MARKER);
    };

    HTMLParser.ActiveFormattingElements.prototype.push = function(elt, attrs) {
        // Scan backwards: if there are already 3 copies of this element
        // before we encounter a marker, then drop the last one
        var count = 0;
        for(var i = this.list.length-1; i >= 0; i--) {
            if (this.list[i] === this.MARKER) break;
            // equal() is defined below
            if (equal(elt, this.list[i], this.attrs[i])) {
                count++;
                if (count === 3) {
                    splice(this.list, i, 1);
                    splice(this.attrs, i, 1);
                    break;
                }
            }
        }


        // Now push the element onto the list
        push(this.list, elt);

        // Copy the attributes and push those on, too
        var attrcopy = [];
        for(var i = 0; i < attrs.length; i++) {
            attrcopy[i] = attrs[i];
        }

        push(this.attrs, attrcopy);

        // This function defines equality of two elements for the purposes
        // of the AFE list.  Note that it compares the new elements
        // attributes to the saved array of attributes associated with
        // the old element because a script could have changed the
        // old element's set of attributes
        function equal(newelt, oldelt, oldattrs) {
            if (newelt.localName !== oldelt.localName) return false;
            if (newelt._numattrs !== oldattrs.length) return false;
            for(var i = 0, n = oldattrs.length; i < n; i++) {
                var oldname = oldattrs[i][0];
                var oldval = oldattrs[i][1];
                if (!newelt.hasAttribute(oldname)) return false;
                if (newelt.getAttribute(oldname) !== oldval) return false;
            }
            return true;
        }
    };

    HTMLParser.ActiveFormattingElements.prototype.clearToMarker = function() {
        for(var i = this.list.length-1; i >= 0; i--) {
            if (this.list[i] === this.MARKER) break;
        }
        if (i < 0) i = 0;
        this.list.length = i;
        this.attrs.length = i;
    };

    // Find and return the last element with the specified tag between the
    // end of the list and the last marker on the list.
    // Used when parsing <a> in_body_mode()
    HTMLParser.ActiveFormattingElements.prototype.findElementByTag = function(tag) {
        for(var i = this.list.length-1; i >= 0; i--) {
            var elt = this.list[i];
            if (elt === this.MARKER) break;
            if (elt.localName === tag) return elt;
        }
        return null;
    };

    HTMLParser.ActiveFormattingElements.prototype.indexOf = function(e) {
        return A.lastIndexOf(this.list, e);
    };

    // Find the element e in the list and remove it
    // Used when parsing <a> in_body()
    HTMLParser.ActiveFormattingElements.prototype.remove = function(e) {
        var idx = A.lastIndexOf(this.list, e);
        if (idx !== -1) {
            splice(this.list, idx, 1);
            splice(this.attrs, idx, 1);
        }
    };

    // Find element a in the list and replace it with element b
    // XXX: Do I need to handle attributes here?
    HTMLParser.ActiveFormattingElements.prototype.replace = function(a, b, attrs) {
        var idx = A.lastIndexOf(this.list, a);
        if (idx !== -1) {
            this.list[idx] = b;
            if (attrs) this.attrs[idx] = attrs;
        }
    };

    // Find a in the list and insert b after it
    // This is only used for insert a bookmark object, so the
    // attrs array doesn't really matter
    HTMLParser.ActiveFormattingElements.prototype.insertAfter = function(a,b) {
        var idx = A.lastIndexOf(this.list, a);
        if (idx !== -1) {
            splice(this.list, idx, 0, b);
            splice(this.attrs, idx, 0, b);
        }
    };




    /***
     * This is the parser factory function. It is the return value of
     * the outer closure that it is defined within.  Most of the parser
     * implementation details are inside this function.
     */
    function HTMLParser(address, fragmentContext, options) {
        /***
         * These are the parser's state variables
         */
        // Scanner state
        var chars = null;
        var numchars = 0;     // Length of chars
        var nextchar = 0;     // Index of next char
        var input_complete = false; // Becomes true when end() called.
        var scanner_skip_newline = false;  // If previous char was CR
        var reentrant_invocations = 0;
        var saved_scanner_state = [];
        var leftovers = "";
        var first_batch = true;
        var paused = 0;   // Becomes non-zero while loading scripts


        // Tokenizer state
        var tokenizer = data_state;     // Current tokenizer state
        var savedTokenizerStates = [];  // Stack of saved states
        var tagnamebuf = [];
        var lasttagname = "";  // holds the target end tag for text states
        var tempbuf = [];
        var attrnamebuf = [];
        var attrvaluebuf = [];
        var commentbuf = [];
        var doctypenamebuf = [];
        var doctypepublicbuf = [];
        var doctypesystembuf = [];
        var attributes = [];
        var is_end_tag = false;

        // Tree builder state
        var parser = initial_mode;                    // Current insertion mode
        var originalInsertionMode = null;             // A saved insertion mode
        var stack = new HTMLParser.ElementStack();    // Stack of open elements
        var afe = new HTMLParser.ActiveFormattingElements(); // mis-nested tags
        var fragment = (fragmentContext!==undefined); // For innerHTML, etc.
        var script_nesting_level = 0;
        var parser_pause_flag = false;
        var head_element_pointer = null;
        var form_element_pointer = null;
        var scripting_enabled = true;
        if (options && options.scripting_enabled === false)
            scripting_enabled = false;
        var frameset_ok = true;
        var force_quirks = false;
        var pending_table_text;
        var text_integration_mode;     // XXX a spec bug workaround?

        // A single run of characters, buffered up to be sent to
        // the parser as a single string.
        var textrun = [];
        var textIncludesNUL = false;
        var ignore_linefeed = false;

        /***
         * This is the parser object that will be the return value of this
         * factory function, which is some 5000 lines below.
         * Note that the variable "parser" is the current state of the
         * parser's state machine.  This variable "htmlparser" is the
         * return value and defines the public API of the parser
         */
        var htmlparser = {
            document: function() {
                // For the fragment case, return the document unwrapped.
                // Otherwise, return a wrapped document
                if (fragment) return doc;
                else return wrap(doc);
            },

            // Internal function used from HTMLScriptElement to pause the
            // parser while a script is being loaded from the network
            pause: function() {
                // print("pausing parser");
                paused++;
            },

            // Called when a script finishes loading
            resume: function() {
                // print("resuming parser");
                paused--;
                // XXX: added this to force a resumption.
                // Is this the right thing to do?
                this.parse("");
            },

            // Parse the HTML text s.
            // The second argument should be true if there is no more
            // text to be parsed, and should be false or omitted otherwise.
            // The second argument must not be set for recursive invocations
            // from document.write()
            parse: function(s, end) {

                // If we're paused, remember the text to parse, but
                // don't parse it now.
                if (paused > 0) {
                    leftovers += s;
                    return;
                }


                if (reentrant_invocations === 0) {
                    // A normal, top-level invocation
                    if (leftovers) {
                        s = leftovers + s;
                        leftovers = "";
                    }

                    // Add a special marker character to the end of
                    // the buffer.  If the scanner is at the end of
                    // the buffer and input_complete is set, then this
                    // character will transform into an EOF token.
                    // Having an actual character that represents EOF
                    // in the character buffer makes lookahead regexp
                    // matching work more easily, and this is
                    // important for character references.
                    if (end) {
                        s += "\uFFFF";
                        input_complete = true;  // Makes scanChars() send EOF
                    }

                    chars = s;
                    numchars = s.length;
                    nextchar = 0;

                    if (first_batch) {
                        // We skip a leading Byte Order Mark (\uFEFF)
                        // on first batch of text we're given
                        first_batch = false;
                        if (S.charCodeAt(chars, 0) === 0xFEFF) nextchar = 1;
                    }

                    reentrant_invocations++;
                    scanChars();
                    leftovers = chars.substring(nextchar, numchars);
                    reentrant_invocations--;
                }
                else {
                    // This is the re-entrant case, which we have to
                    // handle a little differently.
                    reentrant_invocations++;

                    // Save current scanner state
                    saved_scanner_state.push(chars, numchars, nextchar);

                    // Set new scanner state
                    chars = s;
                    numchars = s.length;
                    nextchar = 0;

                    // Now scan as many of these new chars as we can
                    scanChars();

                    leftovers = chars.substring(nextchar, numchars);

                    // restore old scanner state
                    nextchar = saved_scanner_state.pop();
                    numchars = saved_scanner_state.pop();
                    chars = saved_scanner_state.pop();

                    // If there were leftover chars from this invocation
                    // insert them into the pending invocation's buffer
                    // and trim already processed chars at the same time
                    if (leftovers) {
                        chars = leftovers + chars.substring(nextchar);
                        numchars = chars.length;
                        nextchar = 0;
                        leftovers = "";
                    }

                    // Decrement the counter
                    reentrant_invocations--;
                }
            }
        };


        // This is the document we'll be building up
        var doc = new impl.Document(true, address);

        // The document needs to know about the parser, for document.write().
        // This _parser property will be deleted when we're done parsing.
        doc._parser = htmlparser;

        // XXX I think that any document we use this parser on should support
        // scripts. But I may need to configure that through a parser parameter
        // Only documents with windows ("browsing contexts" to be precise)
        // allow scripting.
        doc._scripting_enabled = scripting_enabled;


        /***
         * The actual code of the HTMLParser() factory function begins here.
         */

        if (fragmentContext) { // for innerHTML parsing
            if (fragmentContext.ownerDocument._quirks)
                doc._quirks = true;
            if (fragmentContext.ownerDocument._limitedQuirks)
                doc._limitedQuirks = true;

            // Set the initial tokenizer state
            if (fragmentContext.namespaceURI === HTML_NAMESPACE) {
                switch(fragmentContext.localName) {
                case "title":
                case "textarea":
                    tokenizer = rcdata_state;
                    break;
                case "style":
                case "xmp":
                case "iframe":
                case "noembed":
                case "noframes":
                case "script":
                case "plaintext":
                    tokenizer = plaintext_state;
                    break;
                case "noscript":
                    if (scripting_enabled)
                        tokenizer = plaintext_state;
                }
            }

            var root = doc.createElement("html");
            doc.appendChild(root);
            stack.push(root);
            resetInsertionMode();

            for(var e = fragmentContext; e !== null; e = e.parentElement) {
                if (e instanceof impl.HTMLFormElement) {
                    form_element_pointer = e;
                    break;
                }
            }
        }

        /***
         * Scanner functions
         */
        // Loop through the characters in chars, and pass them one at a time
        // to the tokenizer FSM. Return when no more characters can be processed
        // (This may leave 1 or more characters in the buffer: like a CR
        // waiting to see if the next char is LF, or for states that require
        // lookahead...)
        function scanChars() {
            var codepoint, s, pattern, eof, matched;

            while(nextchar < numchars) {

                // If we just tokenized a </script> tag, then the paused flag
                // may have been set to tell us to stop tokenizing while
                // the script is loading
                if (paused > 0) {
                    return;
                }


                switch(typeof tokenizer.lookahead) {
                case 'undefined':
                    codepoint = S.charCodeAt(chars, nextchar++);
                    if (scanner_skip_newline) {
                        scanner_skip_newline = false;
                        if (codepoint === 0x000A) {
                            nextchar++;
                            continue;
                        }
                    }
                    switch(codepoint) {
                    case 0x000D:
                        // CR always turns into LF, but if the next character
                        // is LF, then that second LF is skipped.
                        if (nextchar < numchars) {
                            if (S.charCodeAt(chars, nextchar) === 0x000A)
                                nextchar++;
                        }
                        else {
                            // We don't know the next char right now, so we
                            // can't check if it is a LF.  So set a flag
                            scanner_skip_newline = true;
                        }

                        // In either case, emit a LF
                        tokenizer(0x000A);

                        break;
                    case 0xFFFF:
                        if (input_complete && nextchar === numchars) {
                            tokenizer(EOF);  // codepoint will be 0xFFFF here
                            break;
                        }
                        /* falls through */
                    default:
                        tokenizer(codepoint);
                        break;
                    }
                    break;

                case 'number':
                    codepoint = S.charCodeAt(chars, nextchar);

                    // The only tokenizer states that require fixed lookahead
                    // only consume alphanum characters, so we don't have
                    // to worry about CR and LF in this case

                    // tokenizer wants n chars of lookahead
                    var n = tokenizer.lookahead;

                    if (n < numchars - nextchar) {
                        // If we can look ahead that far
                        s = substring(chars, nextchar, nextchar+n);
                        eof = false;
                    }
                    else { // if we don't have that many characters
                        if (input_complete) { // If no more are coming
                            // Just return what we have
                            s = substring(chars, nextchar, numchars);
                            eof = true;
                            if (codepoint === 0xFFFF && nextchar === numchars-1)
                                codepoint = EOF;
                        }
                        else {
                            // Return now and wait for more chars later
                            return;
                        }
                    }
                    tokenizer(codepoint, s, eof);
                    break;
                case 'string':
                    codepoint = S.charCodeAt(chars, nextchar);

                    // tokenizer wants characters up to a matching string
                    pattern = tokenizer.lookahead;
                    var pos = S.indexOf(chars, pattern, nextchar);
                    if (pos !== -1) {
                        s = substring(chars, nextchar, pos + pattern.length);
                        eof = false;
                    }
                    else {  // No match
                        // If more characters coming, wait for them
                        if (!input_complete) return;

                        // Otherwise, we've got to return what we've got
                        s = substring(chars, nextchar, numchars);
                        if (codepoint === 0xFFFF && nextchar === numchars-1)
                            codepoint = EOF;
                        eof = true;
                    }

                    // The tokenizer states that require this kind of
                    // lookahead have to be careful to handle CR characters
                    // correctly
                    tokenizer(codepoint, s, eof);
                    break;
                case 'object':
                    codepoint = S.charCodeAt(chars, nextchar);

                    // tokenizer wants characters that match a regexp
                    // The only tokenizer states that use regexp lookahead
                    // are for character entities, and the patterns never
                    // match CR or LF, so we don't need to worry about that
                    // here.

                    // XXX
                    // Ideally, I'd use the non-standard y modifier on
                    // these regexps and set pattern.lastIndex to nextchar.
                    // But v8 and Node don't support /y, so I have to do
                    // the substring below
                    pattern = tokenizer.lookahead;
                    matched = match(substring(chars, nextchar), pattern);
                    if (matched) {
                        // Found a match.
                        // lastIndex now points to the first char after it
                        s = matched[0];
                        eof = false;
                    }
                    else {
                        // No match.  If we're not at the end of input, then
                        // wait for more chars
                        if (!input_complete) return;

                        // Otherwise, pass an empty string.  This is
                        // different than the string-based lookahead
                        // above. Regexp-based lookahead is only used
                        // for character references, and a partial one
                        // will not parse.  Also, a char ref
                        // terminated with EOF will parse in the if
                        // branch above, so here we're dealing with
                        // things that really aren't char refs
                        s = "";
                        eof = true;
                    }

                    tokenizer(codepoint, s, eof);
                    break;
                }
            }
        }


        /***
         * Tokenizer utility functions
         */
        function addAttribute(namebuf,valuebuf) {
            var name = buf2str(namebuf);
            var value;

            // Make sure there isn't already an attribute with this name
            // If there is, ignore this one.
            for(var i = 0; i < attributes.length; i++) {
                if (attributes[i][0] === name) return;
            }

            if (valuebuf) {
                push(attributes, [name, buf2str(valuebuf)]);
            }
            else {
                push(attributes, [name]);
            }
        }

        // Shortcut for simple attributes
        function handleSimpleAttribute() {
            var matched = match(substring(chars, nextchar-1), SIMPLEATTR);
            if (!matched) return false;
            var name = matched[1];
            var value = matched[2];
            var len = value.length;
            switch(value[0]) {
            case '"':
            case "'":
                value = substring(value, 1, len-1);
                nextchar += (matched[0].length-1);
                tokenizer = after_attribute_value_quoted_state;
                break;
            default:
                tokenizer = before_attribute_name_state;
                nextchar += (matched[0].length-1);
                value = substring(value, 0, len-1);
                break;
            }

            // Make sure there isn't already an attribute with this name
            // If there is, ignore this one.
            for(var i = 0; i < attributes.length; i++) {
                if (attributes[i][0] === name) return true;
            }

            push(attributes, [name, value]);
            return true;
        }


        function pushState() { push(savedTokenizerStates, tokenizer); }
        function popState() { tokenizer = pop(savedTokenizerStates); }
        function beginTagName() {
            is_end_tag = false;
            tagnamebuf.length = 0;
            attributes.length = 0;
        }
        function beginEndTagName() {
            is_end_tag = true;
            tagnamebuf.length = 0;
            attributes.length = 0;
        }

        function beginTempBuf() { tempbuf.length = 0; }
        function beginAttrName() { attrnamebuf.length = 0; }
        function beginAttrValue() { attrvaluebuf.length = 0; }
        function beginComment() { commentbuf.length = 0; }
        function beginDoctype() {
            doctypenamebuf.length = 0;
            doctypepublicbuf = null;
            doctypesystembuf = null;
        }
        function beginDoctypePublicId() { doctypepublicbuf = []; }
        function beginDoctypeSystemId() { doctypesystembuf = []; }
        function appendChar(buf, char) { push(buf, char); }
        function forcequirks() { force_quirks = true; }
        function cdataAllowed() {
            return stack.top &&
                stack.top.namespaceURI !== "http://www.w3.org/1999/xhtml";
        }

        // Return true if the codepoints in the specified buffer match the
        // characters of lasttagname
        function appropriateEndTag(buf) {
            if (buf.length !== lasttagname.length) return false;
            for(var i = 0, n = buf.length; i < n; i++) {
                if (buf[i] !== lasttagname.charCodeAt(i)) return false;
            }
            return true;
        }

        function flushText() {
            if (textrun.length > 0) {
                var s = buf2str(textrun);
                textrun.length = 0;

                if (ignore_linefeed) {
                    ignore_linefeed = false;
                    if (s[0] === "\n") s = substring(s, 1);
                    if (s.length === 0) return;
                }

                insertToken(TEXT, s);
                textIncludesNUL = false;
            }
            ignore_linefeed = false;
        }

        // emit a string of chars that match a regexp
        // Returns false if no chars matched.
        function emitCharsWhile(pattern) {
            pattern.lastIndex = nextchar-1;
            var match = exec(pattern, chars)[0];
            if (!match) return false;
            emitCharString(match);
            nextchar += match.length - 1;
            return true;
        }

        // This is used by CDATA sections
        function emitCharString(s) {
            if (textrun.length > 0) flushText();

            if (ignore_linefeed) {
                ignore_linefeed = false;
                if (s[0] === "\n") s = substring(s, 1);
                if (s.length === 0) return;
            }

            insertToken(TEXT, s);
        }

        function emitTag() {
            if (is_end_tag) insertToken(ENDTAG, buf2str(tagnamebuf));
            else {
                // Remember the last open tag we emitted
                var tagname = buf2str(tagnamebuf);
                tagnamebuf.length = 0;
                lasttagname = tagname;
                insertToken(TAG, tagname, attributes);
            }
        }


        // A shortcut: look ahead and if this is a open or close tag
        // in lowercase with no spaces and no attributes, just emit it now.
        function emitSimpleTag() {
            var matched = match(substring(chars, nextchar), SIMPLETAG);
            if (!matched) return false;
            var tagname = matched[2];
            var endtag = matched[1];
            if (endtag) {
                nextchar += (tagname.length+2);
                insertToken(ENDTAG, tagname);
            }
            else {
                nextchar += (tagname.length+1);
                lasttagname = tagname;
                insertToken(TAG, tagname, NOATTRS);
            }
            return true;
        }

        function emitSelfClosingTag() {
            if (is_end_tag) insertToken(ENDTAG, buf2str(tagnamebuf), null, true);
            else {
                insertToken(TAG, buf2str(tagnamebuf), attributes, true);
            }
        }

        function emitDoctype() {
            insertToken(DOCTYPE,
                        buf2str(doctypenamebuf),
                        doctypepublicbuf ? buf2str(doctypepublicbuf) : undefined,
                        doctypesystembuf ? buf2str(doctypesystembuf) : undefined);
        }

        function emitEOF() {
            flushText();
            parser(EOF);  // EOF never goes to insertForeignContent()
        }

        // Insert a token, either using the current parser insertio mode
        // (for HTML stuff) or using the insertForeignToken() method.
        function insertToken(t, value, arg3, arg4) {
            flushText();
            var current = stack.top;

            if (!current || current.namespaceURI === HTML_NAMESPACE) {
                // This is the common case
                parser(t, value, arg3, arg4);
            }
            else {
                // Otherwise we may need to insert this token as foreign content
                if (t !== TAG && t !== TEXT) {
                    insertForeignToken(t, value, arg3, arg4);
                }
                else {
                    // But in some cases we treat it as regular content
                    if ((isMathmlTextIntegrationPoint(current) &&
                         (t === TEXT ||
                          (t === TAG &&
                           value !== "mglyph" && value !== "malignmark"))) ||
                        (t === TAG &&
                         value === "svg" &&
                         current.namespaceURI === MATHML_NAMESPACE &&
                         current.localName === "annotation-xml") ||
                        isHTMLIntegrationPoint(current)) {

                        // XXX: the text_integration_mode stuff is an
                        // attempted bug workaround of mine
                        text_integration_mode = true;
                        parser(t, value, arg3, arg4);
                        text_integration_mode = false;
                    }
                    // Otherwise it is foreign content
                    else {
                        insertForeignToken(t, value, arg3, arg4);
                    }
                }
            }
        }


        /***
         * Tree building utility functions
         */
        function insertComment(data) {
            stack.top.appendChild(doc.createComment(data));
        }

        function insertText(s) {
            if (foster_parent_mode && isA(stack.top, tablesectionrowSet)) {
                fosterParent(doc.createTextNode(s));
            }
            else {
                var lastChild = stack.top.lastChild;
                if (lastChild && lastChild.nodeType === Node.TEXT_NODE) {
                    lastChild.appendData(s);
                }
                else {
                    stack.top.appendChild(doc.createTextNode(s));
                }
            }
        }

        function createHTMLElt(name, attrs) {
            // Create the element this way, rather than with
            // doc.createElement because createElement() does error
            // checking on the element name that we need to avoid here.
            var interfaceName = tagNameToInterfaceName[name] ||
                "HTMLUnknownElement";
            var elt = new impl[interfaceName](doc, name, null);

            if (attrs) {
                for(var i = 0, n = attrs.length; i < n; i++) {
                    // Use the _ version to avoid testing the validity
                    // of the attribute name
                    elt._setAttribute(attrs[i][0], attrs[i][1]);
                }
            }
            // XXX
            // If the element is a resettable form element,
            // run its reset algorithm now
            return elt;
        }

        // The in_table insertion mode turns on this flag, and that makes
        // insertHTMLElement use the foster parenting algorithm for elements
        // tags inside a table
        var foster_parent_mode = false;

        function insertHTMLElement(name, attrs) {
            var elt = createHTMLElt(name, attrs);
            insertElement(elt);

            // XXX
            // If this is a form element, set its form attribute property here

            return elt;
        }

        // Insert the element into the open element or foster parent it
        function insertElement(elt) {
            if (foster_parent_mode && isA(stack.top, tablesectionrowSet)) {
                fosterParent(elt);
            }
            else {
                stack.top.appendChild(elt);
            }

            stack.push(elt);
        }

        function insertForeignElement(name, attrs, ns) {
            var elt = doc.createElementNS(ns, name);
            if (attrs) {
                for(var i = 0, n = attrs.length; i < n; i++) {
                    var attr = attrs[i];
                    if (attr.length == 2)
                        elt._setAttribute(attr[0], attr[1]);
                    else {
                        elt._setAttributeNS(attr[2], attr[0], attr[1]);
                    }
                }
            }

            insertElement(elt);
        }

        function fosterParent(elt) {
            var parent, before;

            for(var i = stack.elements.length-1; i >= 0; i--) {
                if (stack.elements[i] instanceof impl.HTMLTableElement) {
                    parent = stack.elements[i].parentElement;
                    if (parent)
                        before = stack.elements[i];
                    else
                        parent = stack.elements[i-1];

                    break;
                }
            }
            if (!parent) parent = stack.elements[0];

            if (elt.nodeType === Node.TEXT_NODE) {
                var prev;
                if (before) prev = before.previousSibling;
                else prev = parent.lastChild;
                if (prev && prev.nodeType === Node.TEXT_NODE) {
                    prev.appendData(elt.data);
                    return;
                }
            }
            if (before)
                parent.insertBefore(elt, before);
            else
                parent.appendChild(elt);
        }


        function resetInsertionMode() {
            var last = false;
            for(var i = stack.elements.length-1; i >= 0; i--) {
                var node = stack.elements[i];
                if (i === 0) {
                    last = true;
                    node = fragmentContext;
                }
                if (node.namespaceURI === HTML_NAMESPACE) {
                    var tag = node.localName;
                    switch(tag) {
                    case "select":
                        parser = in_select_mode;
                        return;
                    case "tr":
                        parser = in_row_mode;
                        return;
                    case "tbody":
                    case "tfoot":
                    case "thead":
                        parser = in_table_body_mode;
                        return;
                    case "caption":
                        parser = in_caption_mode;
                        return;
                    case "colgroup":
                        parser = in_column_group_mode;
                        return;
                    case "table":
                        parser = in_table_mode;
                        return;
                    case "head": // Not in_head_mode!
                    case "body":
                        parser = in_body_mode;
                        return;
                    case "frameset":
                        parser = in_frameset_mode;
                        return;
                    case "html":
                        parser = before_head_mode;
                        return;
                    default:
                        if (!last && (tag === "td" || tag === "th")) {
                            parser = in_cell_mode;
                            return;
                        }
                    }
                }
                if (last) {
                    parser = in_body_mode;
                    return;
                }
            }
        }


        function parseRawText(name, attrs) {
            insertHTMLElement(name, attrs);
            tokenizer = rawtext_state;
            originalInsertionMode = parser;
            parser = text_mode;
        }

        function parseRCDATA(name, attrs) {
            insertHTMLElement(name, attrs);
            tokenizer = rcdata_state;
            originalInsertionMode = parser;
            parser = text_mode;
        }

        // Make a copy of element i on the list of active formatting
        // elements, using its original attributes, not current
        // attributes (which may have been modified by a script)
        function afeclone(i) {
            return createHTMLElt(afe.list[i].localName, afe.attrs[i]);
        }


        function afereconstruct() {
            if (afe.list.length === 0) return;
            var entry = afe.list[afe.list.length-1];
            // If the last is a marker , do nothing
            if (entry === afe.MARKER) return;
            // Or if it is an open element, do nothing
            if (A.lastIndexOf(stack.elements, entry) !== -1) return;

            // Loop backward through the list until we find a marker or an
            // open element, and then move forward one from there.
            for(var i = afe.list.length-2; i >= 0; i--) {
                entry = afe.list[i];
                if (entry === afe.MARKER) break;
                if (A.lastIndexOf(stack.elements, entry) !== -1) break;
            }

            // Now loop forward, starting from the element after the current
            // one, recreating formatting elements and pushing them back onto
            // the list of open elements
            for(i = i+1; i < afe.list.length; i++) {
                var newelt = afeclone(i);
                insertElement(newelt);
                afe.list[i] = newelt;
            }
        };

        // Used by the adoptionAgency() function
        const BOOKMARK = {localName:"BM"};

        function adoptionAgency(tag) {
            // Let outer loop counter be zero.
            var outer = 0;

            // Outer loop: If outer loop counter is greater than or
            // equal to eight, then abort these steps.
            while(outer < 8) {
                // Increment outer loop counter by one.
                outer++;

                // Let the formatting element be the last element in the list
                // of active formatting elements that: is between the end of
                // the list and the last scope marker in the list, if any, or
                // the start of the list otherwise, and has the same tag name
                // as the token.
                var fmtelt = afe.findElementByTag(tag);

                // If there is no such node, then abort these steps and instead
                // act as described in the "any other end tag" entry below.
                if (!fmtelt) {
                    return false;  // false means handle by the default case
                }

                // Otherwise, if there is such a node, but that node is not in
                // the stack of open elements, then this is a parse error;
                // remove the element from the list, and abort these steps.
                var index = A.lastIndexOf(stack.elements, fmtelt);
                if (index === -1) {
                    afe.remove(fmtelt);
                    return true;   // true means no more handling required
                }

                // Otherwise, if there is such a node, and that node is also in
                // the stack of open elements, but the element is not in scope,
                // then this is a parse error; ignore the token, and abort
                // these steps.
                if (!stack.elementInScope(fmtelt)) {
                    return true;
                }

                // Let the furthest block be the topmost node in the stack of
                // open elements that is lower in the stack than the formatting
                // element, and is an element in the special category. There
                // might not be one.
                var furthestblock = null, furthestblockindex;
                for(var i = index+1; i < stack.elements.length; i++) {
                    if (isA(stack.elements[i], specialSet)) {
                        furthestblock = stack.elements[i];
                        furthestblockindex = i;
                        break;
                    }
                }

                // If there is no furthest block, then the UA must skip the
                // subsequent steps and instead just pop all the nodes from the
                // bottom of the stack of open elements, from the current node
                // up to and including the formatting element, and remove the
                // formatting element from the list of active formatting
                // elements.
                if (!furthestblock) {
                    stack.popElement(fmtelt);
                    afe.remove(fmtelt);
                }
                else {
                    // Let the common ancestor be the element immediately above
                    // the formatting element in the stack of open elements.
                    var ancestor = stack.elements[index-1];

                    // Let a bookmark note the position of the formatting
                    // element in the list of active formatting elements
                    // relative to the elements on either side of it in the
                    // list.
                    afe.insertAfter(fmtelt, BOOKMARK);

                    // Let node and last node be the furthest block.
                    var node = furthestblock;
                    var lastnode = furthestblock;
                    var nodeindex = furthestblockindex;
                    var nodeafeindex;

                    // Let inner loop counter be zero.
                    var inner = 0;

                    // Inner loop: If inner loop counter is greater than
                    // or equal to three, then abort these steps.
                    while(inner < 3) {

                        // Increment inner loop counter by one.
                        inner++;

                        // Let node be the element immediately above node in
                        // the stack of open elements, or if node is no longer
                        // in the stack of open elements (e.g. because it got
                        // removed by the next step), the element that was
                        // immediately above node in the stack of open elements
                        // before node was removed.
                        node = stack.elements[--nodeindex];

                        // If node is not in the list of active formatting
                        // elements, then remove node from the stack of open
                        // elements and then go back to the step labeled inner
                        // loop.
                        nodeafeindex = afe.indexOf(node);
                        if (nodeafeindex === -1) {
                            stack.removeElement(node);
                            continue;
                        }

                        // Otherwise, if node is the formatting element, then go
                        // to the next step in the overall algorithm.
                        if (node === fmtelt) break;

                        // Create an element for the token for which the
                        // element node was created, replace the entry for node
                        // in the list of active formatting elements with an
                        // entry for the new element, replace the entry for
                        // node in the stack of open elements with an entry for
                        // the new element, and let node be the new element.
                        var newelt = afeclone(nodeafeindex);
                        afe.replace(node, newelt);
                        stack.elements[nodeindex] = newelt;
                        node = newelt;

                        // If last node is the furthest block, then move the
                        // aforementioned bookmark to be immediately after the
                        // new node in the list of active formatting elements.
                        if (lastnode === furthestblock) {
                            afe.remove(BOOKMARK);
                            afe.insertAfter(newelt, BOOKMARK);
                        }

                        // Insert last node into node, first removing it from
                        // its previous parent node if any.
                        node.appendChild(lastnode);

                        // Let last node be node.
                        lastnode = node;
                    }

                    // If the common ancestor node is a table, tbody, tfoot,
                    // thead, or tr element, then, foster parent whatever last
                    // node ended up being in the previous step, first removing
                    // it from its previous parent node if any.
                    if (isA(ancestor, tablesectionrowSet)) {
                        fosterParent(lastnode);
                    }
                    // Otherwise, append whatever last node ended up being in
                    // the previous step to the common ancestor node, first
                    // removing it from its previous parent node if any.
                    else {
                        ancestor.appendChild(lastnode);
                    }

                    // Create an element for the token for which the
                    // formatting element was created.
                    var newelt2 = afeclone(afe.indexOf(fmtelt));

                    // Take all of the child nodes of the furthest block and
                    // append them to the element created in the last step.
                    while(furthestblock.hasChildNodes()) {
                        newelt2.appendChild(furthestblock.firstChild);
                    }

                    // Append that new element to the furthest block.
                    furthestblock.appendChild(newelt2);

                    // Remove the formatting element from the list of active
                    // formatting elements, and insert the new element into the
                    // list of active formatting elements at the position of
                    // the aforementioned bookmark.
                    afe.remove(fmtelt);
                    afe.replace(BOOKMARK, newelt2);

                    // Remove the formatting element from the stack of open
                    // elements, and insert the new element into the stack of
                    // open elements immediately below the position of the
                    // furthest block in that stack.
                    stack.removeElement(fmtelt);
                    var pos = A.lastIndexOf(stack.elements, furthestblock);
                    splice(stack.elements, pos+1, 0, newelt2);
                }
            }

            return true;
        }

        // We do this when we get /script in in_text_mode
        function handleScriptEnd() {
            // XXX:
            // This is just a stub implementation right now and doesn't run scripts.
            // Getting this method right involves the event loop, URL resolution
            // script fetching etc. For now I just want to be able to parse
            // documents and test the parser.

            var script = stack.top;
            stack.pop();
            parser = originalInsertionMode;
            script._prepare();
            return;

            // XXX: here is what this method is supposed to do

            // Provide a stable state.

            // Let script be the current node (which will be a script
            // element).

            // Pop the current node off the stack of open elements.

            // Switch the insertion mode to the original insertion mode.

            // Let the old insertion point have the same value as the current
            // insertion point. Let the insertion point be just before the
            // next input character.

            // Increment the parser's script nesting level by one.

            // Prepare the script. This might cause some script to execute,
            // which might cause new characters to be inserted into the
            // tokenizer, and might cause the tokenizer to output more tokens,
            // resulting in a reentrant invocation of the parser.

            // Decrement the parser's script nesting level by one. If the
            // parser's script nesting level is zero, then set the parser
            // pause flag to false.

            // Let the insertion point have the value of the old insertion
            // point. (In other words, restore the insertion point to its
            // previous value. This value might be the "undefined" value.)

            // At this stage, if there is a pending parsing-blocking script,
            // then:

            // If the script nesting level is not zero:

            //     Set the parser pause flag to true, and abort the processing
            //     of any nested invocations of the tokenizer, yielding
            //     control back to the caller. (Tokenization will resume when
            //     the caller returns to the "outer" tree construction stage.)

            //     The tree construction stage of this particular parser is
            //     being called reentrantly, say from a call to
            //     document.write().

            // Otherwise:

            //         Run these steps:

            //             Let the script be the pending parsing-blocking
            //             script. There is no longer a pending
            //             parsing-blocking script.

            //             Block the tokenizer for this instance of the HTML
            //             parser, such that the event loop will not run tasks
            //             that invoke the tokenizer.

            //             If the parser's Document has a style sheet that is
            //             blocking scripts or the script's "ready to be
            //             parser-executed" flag is not set: spin the event
            //             loop until the parser's Document has no style sheet
            //             that is blocking scripts and the script's "ready to
            //             be parser-executed" flag is set.

            //             Unblock the tokenizer for this instance of the HTML
            //             parser, such that tasks that invoke the tokenizer
            //             can again be run.

            //             Let the insertion point be just before the next
            //             input character.

            //             Increment the parser's script nesting level by one
            //             (it should be zero before this step, so this sets
            //             it to one).

            //             Execute the script.

            //             Decrement the parser's script nesting level by
            //             one. If the parser's script nesting level is zero
            //             (which it always should be at this point), then set
            //             the parser pause flag to false.

            //             Let the insertion point be undefined again.

            //             If there is once again a pending parsing-blocking
            //             script, then repeat these steps from step 1.


        }

        function stopParsing() {
            // XXX This is just a temporary implementation to get the parser working.
            // A full implementation involves scripts and events and the event loop.

            // Remove the link from document to parser.
            // This is instead of "set the insertion point to undefined".
            // It means that document.write() can't write into the doc anymore.
            delete doc._parser;

            stack.elements.length = 0;  // pop everything off

            // If there is a window object associated with the document
            // then trigger an load event on it
            if (doc.defaultView) {
                doc.defaultView.dispatchEvent(new impl.Event("load",{}));
            }

        }

        /****
         * Tokenizer states
         */

        /**
         * This file was partially mechanically generated from
         * http://www.whatwg.org/specs/web-apps/current-work/multipage/tokenization.html
         *
         * After mechanical conversion, it was further converted from
         * prose to JS by hand, but the intent is that it is a very
         * faithful rendering of the HTML tokenization spec in
         * JavaScript.
         *
         * It is not a goal of this tokenizer to detect or report
         * parse errors.
         *
         * XXX The tokenizer is supposed to work with straight UTF32
         * codepoints. But I don't think it has any dependencies on
         * any character outside of the BMP so I think it is safe to
         * pass it UTF16 characters. I don't think it will ever change
         * state in the middle of a surrogate pair.
         */

        /*
         * Each state is represented by a function.  For most states, the
         * scanner simply passes the next character (as an integer
         * codepoint) to the current state function and automatically
         * consumes the character.  If the state function can't process
         * the character it can call pushback() to push it back to the
         * scanner.
         *
         * Some states require lookahead, though.  If a state function has
         * a lookahead property, then it is invoked differently.  In this
         * case, the scanner invokes the function with 3 arguments: 1) the
         * next codepoint 2) a string of lookahead text 3) a boolean that
         * is true if the lookahead goes all the way to the EOF. (XXX
         * actually maybe this third is not necessary... the lookahead
         * could just include \uFFFF?)
         *
         * If the lookahead property of a state function is an integer, it
         * specifies the number of characters required. If it is a string,
         * then the scanner will scan for that string and return all
         * characters up to and including that sequence, or up to EOF.  If
         * the lookahead property is a regexp, then the scanner will match
         * the regexp at the current point and return the matching string.
         *
         * States that require lookahead are responsible for explicitly
         * consuming the characters they process. They do this by
         * incrementing nextchar by the number of processed characters.
         */

        function data_state(c) {
            switch(c) {
            case 0x0026: //  AMPERSAND
                tokenizer = character_reference_in_data_state;
                break;
            case 0x003C: //  LESS-THAN SIGN
                if (emitSimpleTag()) // Shortcut for <p>, <dl>, </div> etc.
                    break;
                tokenizer = tag_open_state;
                break;
            case 0x0000: //  NULL
                // Usually null characters emitted by the tokenizer will be
                // ignored by the tree builder, but sometimes they'll be
                // converted to \uFFFD.  I don't want to have the search every
                // string emitted to replace NULs, so I'll set a flag
                // if I've emitted a NUL.
                push(textrun,c);
                textIncludesNUL = true;
                break;
            case EOF:
                emitEOF();
                break;
            default:
                // Instead of just pushing a single character and then
                // coming back to the very same place, lookahead and
                // emit everything we can at once.
                emitCharsWhile(DATATEXT) || push(textrun, c);
                break;
            }
        }

        function character_reference_in_data_state(c, lookahead, eof) {
            var char = parseCharRef(lookahead, false);
            if (char !== null) {
                if (typeof char === "number") push(textrun,char);
                else pushAll(textrun, char);  // An array of characters
            }
            else
                push(textrun,0x0026); // AMPERSAND;

            tokenizer = data_state;
        }
        character_reference_in_data_state.lookahead = CHARREF;

        function rcdata_state(c) {
            // Save the open tag so we can find a matching close tag
            switch(c) {
            case 0x0026: //  AMPERSAND
                tokenizer = character_reference_in_rcdata_state;
                break;
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = rcdata_less_than_sign_state;
                break;
            case 0x0000: //  NULL
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                textIncludesNUL = true;
                break;
            case EOF:
                emitEOF();
                break;
            default:
                push(textrun,c);
                break;
            }
        }

        function character_reference_in_rcdata_state(c, lookahead, eof) {
            var char = parseCharRef(lookahead, false);
            if (char !== null) {
                if (typeof char === "number") push(textrun,char);
                else pushAll(textrun, char);  // An array of characters
            }
            else
                push(textrun,0x0026); // AMPERSAND;

            tokenizer = rcdata_state;
        }
        character_reference_in_rcdata_state.lookahead = CHARREF;

        function rawtext_state(c) {
            switch(c) {
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = rawtext_less_than_sign_state;
                break;
            case 0x0000: //  NULL
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                emitEOF();
                break;
            default:
                emitCharsWhile(RAWTEXT) || push(textrun, c);
                break;
            }
        }

        function script_data_state(c) {
            switch(c) {
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = script_data_less_than_sign_state;
                break;
            case 0x0000: //  NULL
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                emitEOF();
                break;
            default:
                emitCharsWhile(RAWTEXT) || push(textrun, c);
                break;
            }
        }

        function plaintext_state(c) {
            switch(c) {
            case 0x0000: //  NULL
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                emitEOF();
                break;
            default:
                emitCharsWhile(PLAINTEXT) || push(textrun, c);
                break;
            }
        }

        function tag_open_state(c) {
            switch(c) {
            case 0x0021: //  EXCLAMATION MARK
                tokenizer = markup_declaration_open_state;
                break;
            case 0x002F: //  SOLIDUS
                tokenizer = end_tag_open_state;
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                c += 0x20;  // to lowercase
                /* falls through */

            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                beginTagName();
                appendChar(tagnamebuf, c);
                tokenizer = tag_name_state;
                break;
            case 0x003F: //  QUESTION MARK
                nextchar--;  // pushback
                tokenizer = bogus_comment_state;
                break;
            default:
                push(textrun,0x003C); // LESS-THAN SIGN
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            }
        }

        function end_tag_open_state(c) {
            switch(c) {
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                c += 0x20; // to lowercase
                /* falls through */

            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                beginEndTagName();
                appendChar(tagnamebuf, c);
                tokenizer = tag_name_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                break;
            case EOF:
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,0x002F); // SOLIDUS
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                nextchar--;  // pushback
                tokenizer = bogus_comment_state;
                break;
            }
        }

        function tag_name_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = before_attribute_name_state;
                break;
            case 0x002F: //  SOLIDUS
                tokenizer = self_closing_start_tag_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitTag();
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                appendChar(tagnamebuf, c + 0x0020);
                break;
            case 0x0000: //  NULL
                appendChar(tagnamebuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(tagnamebuf, c);
                // appendCharsWhile(tagnamebuf, TAGNAMECHARS) || appendChar(tagnamebuf, c);
                break;
            }
        }

        function rcdata_less_than_sign_state(c) {
            /* identical to the RAWTEXT less-than sign state, except s/RAWTEXT/RCDATA/g */
            if (c === 0x002F) {  //  SOLIDUS
                beginTempBuf();
                tokenizer = rcdata_end_tag_open_state;
            }
            else {
                push(textrun,0x003C); // LESS-THAN SIGN
                nextchar--;  // pushback
                tokenizer = rcdata_state;
            }
        }

        function rcdata_end_tag_open_state(c) {
            /* identical to the RAWTEXT (and Script data) end tag open state, except s/RAWTEXT/RCDATA/g */
            switch(c) {
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                beginEndTagName();
                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                tokenizer = rcdata_end_tag_name_state;
                break;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                beginEndTagName();
                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                tokenizer = rcdata_end_tag_name_state;
                break;
            default:
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,0x002F); // SOLIDUS
                nextchar--;  // pushback
                tokenizer = rcdata_state;
                break;
            }
        }

        function rcdata_end_tag_name_state(c) {
            /* identical to the RAWTEXT (and Script data) end tag name state, except s/RAWTEXT/RCDATA/g */
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = before_attribute_name_state;
                    return;
                }
                break;
            case 0x002F: //  SOLIDUS
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = self_closing_start_tag_state;
                    return;
                }
                break;
            case 0x003E: //  GREATER-THAN SIGN
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = data_state;
                    emitTag();
                    return;
                }
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:

                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                return;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:

                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                return;
            default:
                break;
            }

            // If we don't return in one of the cases above, then this was not
            // an appropriately matching close tag, so back out by emitting all
            // the characters as text
            push(textrun,0x003C); // LESS-THAN SIGN
            push(textrun,0x002F); // SOLIDUS
            pushAll(textrun, tempbuf);
            nextchar--;  // pushback
            tokenizer = rcdata_state;
        }

        function rawtext_less_than_sign_state(c) {
            /* identical to the RCDATA less-than sign state, except s/RCDATA/RAWTEXT/g
             */
            if (c === 0x002F) { //  SOLIDUS
                beginTempBuf();
                tokenizer = rawtext_end_tag_open_state;
            }
            else {
                push(textrun,0x003C); // LESS-THAN SIGN
                nextchar--;  // pushback
                tokenizer = rawtext_state;
            }
        }

        function rawtext_end_tag_open_state(c) {
            /* identical to the RCDATA (and Script data) end tag open state, except s/RCDATA/RAWTEXT/g */
            switch(c) {
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                beginEndTagName();
                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                tokenizer = rawtext_end_tag_name_state;
                break;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                beginEndTagName();
                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                tokenizer = rawtext_end_tag_name_state;
                break;
            default:
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,0x002F); // SOLIDUS
                nextchar--;  // pushback
                tokenizer = rawtext_state;
                break;
            }
        }

        function rawtext_end_tag_name_state(c) {
            /* identical to the RCDATA (and Script data) end tag name state, except s/RCDATA/RAWTEXT/g */
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = before_attribute_name_state;
                    return;
                }
                break;
            case 0x002F: //  SOLIDUS
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = self_closing_start_tag_state;
                    return;
                }
                break;
            case 0x003E: //  GREATER-THAN SIGN
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = data_state;
                    emitTag();
                    return;
                }
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                return;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                return;
            default:
                break;
            }

            // If we don't return in one of the cases above, then this was not
            // an appropriately matching close tag, so back out by emitting all
            // the characters as text
            push(textrun,0x003C); // LESS-THAN SIGN
            push(textrun,0x002F); // SOLIDUS
            pushAll(textrun,tempbuf);
            nextchar--;  // pushback
            tokenizer = rawtext_state;
        }

        function script_data_less_than_sign_state(c) {
            switch(c) {
            case 0x002F: //  SOLIDUS
                beginTempBuf();
                tokenizer = script_data_end_tag_open_state;
                break;
            case 0x0021: //  EXCLAMATION MARK
                tokenizer = script_data_escape_start_state;
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,0x0021); // EXCLAMATION MARK
                break;
            default:
                push(textrun,0x003C); // LESS-THAN SIGN
                nextchar--;  // pushback
                tokenizer = script_data_state;
                break;
            }
        }

        function script_data_end_tag_open_state(c) {
            /* identical to the RCDATA (and RAWTEXT) end tag open state, except s/RCDATA/Script data/g */
            switch(c) {
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                beginEndTagName();
                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                tokenizer = script_data_end_tag_name_state;
                break;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                beginEndTagName();
                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                tokenizer = script_data_end_tag_name_state;
                break;
            default:
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,0x002F); // SOLIDUS
                nextchar--;  // pushback
                tokenizer = script_data_state;
                break;
            }
        }

        function script_data_end_tag_name_state(c) {
            /* identical to the RCDATA (and RAWTEXT) end tag name state, except s/RCDATA/Script data/g */
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = before_attribute_name_state;
                    return;
                }
                break;
            case 0x002F: //  SOLIDUS
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = self_closing_start_tag_state;
                    return;
                }
                break;
            case 0x003E: //  GREATER-THAN SIGN
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = data_state;
                    emitTag();
                    return;
                }
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:

                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                return;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:

                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                return;
            default:
                break;
            }

            // If we don't return in one of the cases above, then this was not
            // an appropriately matching close tag, so back out by emitting all
            // the characters as text
            push(textrun,0x003C); // LESS-THAN SIGN
            push(textrun,0x002F); // SOLIDUS
            pushAll(textrun,tempbuf);
            nextchar--;  // pushback
            tokenizer = script_data_state;
        }

        function script_data_escape_start_state(c) {
            if (c === 0x002D) { //  HYPHEN-MINUS
                tokenizer = script_data_escape_start_dash_state;
                push(textrun,0x002D); // HYPHEN-MINUS
            }
            else {
                nextchar--;  // pushback
                tokenizer = script_data_state;
            }
        }

        function script_data_escape_start_dash_state(c) {
            if (c === 0x002D) { //  HYPHEN-MINUS
                tokenizer = script_data_escaped_dash_dash_state;
                push(textrun,0x002D); // HYPHEN-MINUS
            }
            else {
                nextchar--;  // pushback
                tokenizer = script_data_state;
            }
        }

        function script_data_escaped_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = script_data_escaped_dash_state;
                push(textrun,0x002D); // HYPHEN-MINUS
                break;
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = script_data_escaped_less_than_sign_state;
                break;
            case 0x0000: //  NULL
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                push(textrun,c);
                break;
            }
        }

        function script_data_escaped_dash_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = script_data_escaped_dash_dash_state;
                push(textrun,0x002D); // HYPHEN-MINUS
                break;
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = script_data_escaped_less_than_sign_state;
                break;
            case 0x0000: //  NULL
                tokenizer = script_data_escaped_state;
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                tokenizer = script_data_escaped_state;
                push(textrun,c);
                break;
            }
        }

        function script_data_escaped_dash_dash_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                push(textrun,0x002D); // HYPHEN-MINUS
                break;
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = script_data_escaped_less_than_sign_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = script_data_state;
                push(textrun,0x003E); // GREATER-THAN SIGN
                break;
            case 0x0000: //  NULL
                tokenizer = script_data_escaped_state;
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                tokenizer = script_data_escaped_state;
                push(textrun,c);
                break;
            }
        }

        function script_data_escaped_less_than_sign_state(c) {
            switch(c) {
            case 0x002F: //  SOLIDUS
                beginTempBuf();
                tokenizer = script_data_escaped_end_tag_open_state;
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                beginTempBuf();
                appendChar(tempbuf, c + 0x0020);
                tokenizer = script_data_double_escape_start_state;
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,c);
                break;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                beginTempBuf();
                appendChar(tempbuf, c);
                tokenizer = script_data_double_escape_start_state;
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,c);
                break;
            default:
                push(textrun,0x003C); // LESS-THAN SIGN
                nextchar--;  // pushback
                tokenizer = script_data_escaped_state;
                break;
            }
        }

        function script_data_escaped_end_tag_open_state(c) {
            switch(c) {
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                beginEndTagName();
                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                tokenizer = script_data_escaped_end_tag_name_state;
                break;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                beginEndTagName();
                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                tokenizer = script_data_escaped_end_tag_name_state;
                break;
            default:
                push(textrun,0x003C); // LESS-THAN SIGN
                push(textrun,0x002F); // SOLIDUS
                nextchar--;  // pushback
                tokenizer = script_data_escaped_state;
                break;
            }
        }

        function script_data_escaped_end_tag_name_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = before_attribute_name_state;
                    return;
                }
                break;
            case 0x002F: //  SOLIDUS
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = self_closing_start_tag_state;
                    return;
                }
                break;
            case 0x003E: //  GREATER-THAN SIGN
                if (appropriateEndTag(tagnamebuf)) {
                    tokenizer = data_state;
                    emitTag();
                    return;
                }
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                appendChar(tagnamebuf, c + 0x0020);
                appendChar(tempbuf, c);
                return;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                appendChar(tagnamebuf, c);
                appendChar(tempbuf, c);
                return;
            default:
                break;
            }

            // We get here in the default case, and if the closing tagname
            // is not an appropriate tagname.
            push(textrun,0x003C); // LESS-THAN SIGN
            push(textrun,0x002F); // SOLIDUS
            pushAll(textrun,tempbuf);
            nextchar--;  // pushback
            tokenizer = script_data_escaped_state;
        }

        function script_data_double_escape_start_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
            case 0x002F: //  SOLIDUS
            case 0x003E: //  GREATER-THAN SIGN
                if (buf2str(tempbuf) === "script") {
                    tokenizer = script_data_double_escaped_state;
                }
                else {
                    tokenizer = script_data_escaped_state;
                }
                push(textrun,c);
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                appendChar(tempbuf, c + 0x0020);
                push(textrun,c);
                break;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                appendChar(tempbuf, c);
                push(textrun,c);
                break;
            default:
                nextchar--;  // pushback
                tokenizer = script_data_escaped_state;
                break;
            }
        }

        function script_data_double_escaped_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = script_data_double_escaped_dash_state;
                push(textrun,0x002D); // HYPHEN-MINUS
                break;
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = script_data_double_escaped_less_than_sign_state;
                push(textrun,0x003C); // LESS-THAN SIGN
                break;
            case 0x0000: //  NULL
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                push(textrun,c);
                break;
            }
        }

        function script_data_double_escaped_dash_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = script_data_double_escaped_dash_dash_state;
                push(textrun,0x002D); // HYPHEN-MINUS
                break;
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = script_data_double_escaped_less_than_sign_state;
                push(textrun,0x003C); // LESS-THAN SIGN
                break;
            case 0x0000: //  NULL
                tokenizer = script_data_double_escaped_state;
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                tokenizer = script_data_double_escaped_state;
                push(textrun,c);
                break;
            }
        }

        function script_data_double_escaped_dash_dash_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                push(textrun,0x002D); // HYPHEN-MINUS
                break;
            case 0x003C: //  LESS-THAN SIGN
                tokenizer = script_data_double_escaped_less_than_sign_state;
                push(textrun,0x003C); // LESS-THAN SIGN
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = script_data_state;
                push(textrun,0x003E); // GREATER-THAN SIGN
                break;
            case 0x0000: //  NULL
                tokenizer = script_data_double_escaped_state;
                push(textrun,0xFFFD); // REPLACEMENT CHARACTER
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                tokenizer = script_data_double_escaped_state;
                push(textrun,c);
                break;
            }
        }

        function script_data_double_escaped_less_than_sign_state(c) {
            if (c === 0x002F) { //  SOLIDUS
                beginTempBuf();
                tokenizer = script_data_double_escape_end_state;
                push(textrun,0x002F); // SOLIDUS
            }
            else {
                nextchar--;  // pushback
                tokenizer = script_data_double_escaped_state;
            }
        }

        function script_data_double_escape_end_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
            case 0x002F: //  SOLIDUS
            case 0x003E: //  GREATER-THAN SIGN
                if (buf2str(tempbuf) === "script") {
                    tokenizer = script_data_escaped_state;
                }
                else {
                    tokenizer = script_data_double_escaped_state;
                }
                push(textrun,c);
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                appendChar(tempbuf, c + 0x0020);
                push(textrun,c);
                break;
            case 0x0061:  // [a-z]
            case 0x0062:case 0x0063:case 0x0064:case 0x0065:case 0x0066:
            case 0x0067:case 0x0068:case 0x0069:case 0x006A:case 0x006B:
            case 0x006C:case 0x006D:case 0x006E:case 0x006F:case 0x0070:
            case 0x0071:case 0x0072:case 0x0073:case 0x0074:case 0x0075:
            case 0x0076:case 0x0077:case 0x0078:case 0x0079:case 0x007A:
                appendChar(tempbuf, c);
                push(textrun,c);
                break;
            default:
                nextchar--;  // pushback
                tokenizer = script_data_double_escaped_state;
                break;
            }
        }

        function before_attribute_name_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                /* Ignore the character. */
                break;
            case 0x002F: //  SOLIDUS
                tokenizer = self_closing_start_tag_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitTag();
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                beginAttrName();
                appendChar(attrnamebuf, c + 0x0020);
                tokenizer = attribute_name_state;
                break;
            case 0x0000: //  NULL
                beginAttrName();
                appendChar(attrnamebuf, 0xFFFD);
                tokenizer = attribute_name_state;
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            case 0x0022: //  QUOTATION MARK
            case 0x0027: //  APOSTROPHE
            case 0x003C: //  LESS-THAN SIGN
            case 0x003D: //  EQUALS SIGN
                /* falls through */
            default:
                if (handleSimpleAttribute()) break;
                beginAttrName();
                appendChar(attrnamebuf, c);
                tokenizer = attribute_name_state;
                break;
            }
        }

        function attribute_name_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = after_attribute_name_state;
                break;
            case 0x002F: //  SOLIDUS
                addAttribute(attrnamebuf);
                tokenizer = self_closing_start_tag_state;
                break;
            case 0x003D: //  EQUALS SIGN
                beginAttrValue();
                tokenizer = before_attribute_value_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                addAttribute(attrnamebuf);
                tokenizer = data_state;
                emitTag();
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                appendChar(attrnamebuf, c + 0x0020);
                break;
            case 0x0000: //  NULL
                appendChar(attrnamebuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            case 0x0022: //  QUOTATION MARK
            case 0x0027: //  APOSTROPHE
            case 0x003C: //  LESS-THAN SIGN
                /* falls through */
            default:
                appendChar(attrnamebuf, c);
                break;
            }
        }

        function after_attribute_name_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                /* Ignore the character. */
                break;
            case 0x002F: //  SOLIDUS
                addAttribute(attrnamebuf);
                tokenizer = self_closing_start_tag_state;
                break;
            case 0x003D: //  EQUALS SIGN
                beginAttrValue();
                tokenizer = before_attribute_value_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                addAttribute(attrnamebuf);
                emitTag();
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                addAttribute(attrnamebuf);
                beginAttrName();
                appendChar(attrnamebuf, c + 0x0020);
                tokenizer = attribute_name_state;
                break;
            case 0x0000: //  NULL
                addAttribute(attrnamebuf);
                beginAttrName();
                appendChar(attrnamebuf, 0xFFFD);
                tokenizer = attribute_name_state;
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            case 0x0022: //  QUOTATION MARK
            case 0x0027: //  APOSTROPHE
            case 0x003C: //  LESS-THAN SIGN
                /* falls through */
            default:
                addAttribute(attrnamebuf);
                beginAttrName();
                appendChar(attrnamebuf, c);
                tokenizer = attribute_name_state;
                break;
            }
        }

        function before_attribute_value_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                /* Ignore the character. */
                break;
            case 0x0022: //  QUOTATION MARK
                tokenizer = attribute_value_double_quoted_state;
                break;
            case 0x0026: //  AMPERSAND
                nextchar--;  // pushback
                tokenizer = attribute_value_unquoted_state;
                break;
            case 0x0027: //  APOSTROPHE
                tokenizer = attribute_value_single_quoted_state;
                break;
            case 0x0000: //  NULL
                appendChar(attrvaluebuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                tokenizer = attribute_value_unquoted_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                addAttribute(attrnamebuf);
                emitTag();
                tokenizer = data_state;
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            case 0x003C: //  LESS-THAN SIGN
            case 0x003D: //  EQUALS SIGN
            case 0x0060: //  GRAVE ACCENT
                /* falls through */
            default:
                appendChar(attrvaluebuf, c);
                tokenizer = attribute_value_unquoted_state;
                break;
            }
        }

        function attribute_value_double_quoted_state(c) {
            switch(c) {
            case 0x0022: //  QUOTATION MARK
                addAttribute(attrnamebuf, attrvaluebuf);
                tokenizer = after_attribute_value_quoted_state;
                break;
            case 0x0026: //  AMPERSAND
                pushState();
                tokenizer = character_reference_in_attribute_value_state;
                break;
            case 0x0000: //  NULL
                appendChar(attrvaluebuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(attrvaluebuf, c);
                // appendCharsWhile(attrvaluebuf, DBLQUOTEATTRVAL);
                break;
            }
        }

        function attribute_value_single_quoted_state(c) {
            switch(c) {
            case 0x0027: //  APOSTROPHE
                addAttribute(attrnamebuf, attrvaluebuf);
                tokenizer = after_attribute_value_quoted_state;
                break;
            case 0x0026: //  AMPERSAND
                pushState();
                tokenizer = character_reference_in_attribute_value_state;
                break;
            case 0x0000: //  NULL
                appendChar(attrvaluebuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(attrvaluebuf, c);
                // appendCharsWhile(attrvaluebuf, SINGLEQUOTEATTRVAL);
                break;
            }
        }

        function attribute_value_unquoted_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                addAttribute(attrnamebuf, attrvaluebuf);
                tokenizer = before_attribute_name_state;
                break;
            case 0x0026: //  AMPERSAND
                pushState();
                tokenizer = character_reference_in_attribute_value_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                addAttribute(attrnamebuf, attrvaluebuf);
                tokenizer = data_state;
                emitTag();
                break;
            case 0x0000: //  NULL
                appendChar(attrvaluebuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            case 0x0022: //  QUOTATION MARK
            case 0x0027: //  APOSTROPHE
            case 0x003C: //  LESS-THAN SIGN
            case 0x003D: //  EQUALS SIGN
            case 0x0060: //  GRAVE ACCENT
                /* falls through */
            default:
                appendChar(attrvaluebuf, c);
                // appendCharsWhile(attrvaluebuf, UNQUOTEDATTRVAL);
                break;
            }
        }

        function character_reference_in_attribute_value_state(c, lookahead, eof) {
            var char = parseCharRef(lookahead, true);
            if (char !== null) {
                if (typeof char === "number")
                    appendChar(attrvaluebuf, char);
                else {
                    // An array of numbers
                    for(var i = 0; i < char.length; i++) {
                        appendChar(attrvaluebuf, char[i]);
                    }
                }
            }
            else {
                appendChar(attrvaluebuf, 0x0026); // AMPERSAND;
            }

            popState();
        }
        character_reference_in_attribute_value_state.lookahead = ATTRCHARREF;

        function after_attribute_value_quoted_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = before_attribute_name_state;
                break;
            case 0x002F: //  SOLIDUS
                tokenizer = self_closing_start_tag_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitTag();
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                nextchar--;  // pushback
                tokenizer = before_attribute_name_state;
                break;
            }
        }

        function self_closing_start_tag_state(c) {
            switch(c) {
            case 0x003E: //  GREATER-THAN SIGN
                // Set the <i>self-closing flag</i> of the current tag token.
                tokenizer = data_state;
                emitSelfClosingTag(true);
                break;
            case EOF:
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                nextchar--;  // pushback
                tokenizer = before_attribute_name_state;
                break;
            }
        }

        function bogus_comment_state(c, lookahead, eof) {
            var len = lookahead.length;

            if (eof) {
                nextchar += len-1;  // don't consume the eof
            }
            else {
                nextchar += len;
            }

            var comment = substring(lookahead, 0, len-1);

            comment = replace(comment, /\u0000/g,"\uFFFD");
            comment = replace(comment, /\u000D\u000A/g,"\u000A");
            comment = replace(comment, /\u000D/g,"\u000A");

            insertToken(COMMENT, comment);
            tokenizer = data_state;
        }
        bogus_comment_state.lookahead = ">";

        function markup_declaration_open_state(c, lookahead, eof) {
            if (lookahead[0] === "-" && lookahead[1] === "-") {
                nextchar += 2;
                beginComment();
                tokenizer = comment_start_state;
                return;
            }

            if (toUpperCase(lookahead) === "DOCTYPE") {
                nextchar += 7;
                tokenizer = doctype_state;
            }
            else if (lookahead === "[CDATA[" && cdataAllowed()) {
                nextchar += 7;
                tokenizer = cdata_section_state;
            }
            else {
                tokenizer = bogus_comment_state;
            }
        }
        markup_declaration_open_state.lookahead = 7;

        function comment_start_state(c) {
            beginComment();
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = comment_start_dash_state;
                break;
            case 0x0000: //  NULL
                appendChar(commentbuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                tokenizer = comment_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                insertToken(COMMENT, buf2str(commentbuf));
                break; /* see comment in comment end state */
            case EOF:
                insertToken(COMMENT, buf2str(commentbuf));
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(commentbuf, c);
                tokenizer = comment_state;
                break;
            }
        }

        function comment_start_dash_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = comment_end_state;
                break;
            case 0x0000: //  NULL
                appendChar(commentbuf, 0x002D /* HYPHEN-MINUS */);
                appendChar(commentbuf, 0xFFFD);
                tokenizer = comment_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                insertToken(COMMENT, buf2str(commentbuf));
                break;
            case EOF:
                insertToken(COMMENT, buf2str(commentbuf));
                nextchar--;  // pushback
                tokenizer = data_state;
                break; /* see comment in comment end state */
            default:
                appendChar(commentbuf, 0x002D /* HYPHEN-MINUS */);
                appendChar(commentbuf, c);
                tokenizer = comment_state;
                break;
            }
        }

        function comment_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = comment_end_dash_state;
                break;
            case 0x0000: //  NULL
                appendChar(commentbuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case EOF:
                insertToken(COMMENT, buf2str(commentbuf));
                nextchar--;  // pushback
                tokenizer = data_state;
                break; /* see comment in comment end state */
            default:
                appendChar(commentbuf, c);
                break;
            }
        }

        function comment_end_dash_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                tokenizer = comment_end_state;
                break;
            case 0x0000: //  NULL
                appendChar(commentbuf, 0x002D /* HYPHEN-MINUS */);
                appendChar(commentbuf, 0xFFFD);
                tokenizer = comment_state;
                break;
            case EOF:
                insertToken(COMMENT, buf2str(commentbuf));
                nextchar--;  // pushback
                tokenizer = data_state;
                break; /* see comment in comment end state */
            default:
                appendChar(commentbuf, 0x002D /* HYPHEN-MINUS */);
                appendChar(commentbuf, c);
                tokenizer = comment_state;
                break;
            }
        }

        function comment_end_state(c) {
            switch(c) {
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                insertToken(COMMENT, buf2str(commentbuf));
                break;
            case 0x0000: //  NULL
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0xFFFD);
                tokenizer = comment_state;
                break;
            case 0x0021: //  EXCLAMATION MARK
                tokenizer = comment_end_bang_state;
                break;
            case 0x002D: //  HYPHEN-MINUS
                appendChar(commentbuf, 0x002D);
                break;
            case EOF:
                insertToken(COMMENT, buf2str(commentbuf));
                nextchar--;  // pushback
                tokenizer = data_state;
                break; /* For security reasons: otherwise, hostile user could put a script in a comment e.g. in a blog comment and then DOS the server so that the end tag isn't read, and then the commented script tag would be treated as live code */
            default:
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, c);
                tokenizer = comment_state;
                break;
            }
        }

        function comment_end_bang_state(c) {
            switch(c) {
            case 0x002D: //  HYPHEN-MINUS
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x0021);
                tokenizer = comment_end_dash_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                insertToken(COMMENT, buf2str(commentbuf));
                break;
            case 0x0000: //  NULL
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x0021);
                appendChar(commentbuf, 0xFFFD);
                tokenizer = comment_state;
                break;
            case EOF:
                insertToken(COMMENT, buf2str(commentbuf));
                nextchar--;  // pushback
                tokenizer = data_state;
                break; /* see comment in comment end state */
            default:
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x002D);
                appendChar(commentbuf, 0x0021);
                appendChar(commentbuf, c);
                tokenizer = comment_state;
                break;
            }
        }

        function doctype_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = before_doctype_name_state;
                break;
            case EOF:
                beginDoctype();
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                nextchar--;  // pushback
                tokenizer = before_doctype_name_state;
                break;
            }
        }

        function before_doctype_name_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                /* Ignore the character. */
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                beginDoctype();
                appendChar(doctypenamebuf, c + 0x0020);
                tokenizer = doctype_name_state;
                break;
            case 0x0000: //  NULL
                beginDoctype();
                appendChar(doctypenamebuf, 0xFFFD);
                tokenizer = doctype_name_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                beginDoctype();
                tokenizer = data_state;
                forcequirks();
                emitDoctype();
                break;
            case EOF:
                beginDoctype();
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                beginDoctype();
                appendChar(doctypenamebuf, c);
                tokenizer = doctype_name_state;
                break;
            }
        }

        function doctype_name_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = after_doctype_name_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitDoctype();
                break;
            case 0x0041:  // [A-Z]
            case 0x0042:case 0x0043:case 0x0044:case 0x0045:case 0x0046:
            case 0x0047:case 0x0048:case 0x0049:case 0x004A:case 0x004B:
            case 0x004C:case 0x004D:case 0x004E:case 0x004F:case 0x0050:
            case 0x0051:case 0x0052:case 0x0053:case 0x0054:case 0x0055:
            case 0x0056:case 0x0057:case 0x0058:case 0x0059:case 0x005A:
                appendChar(doctypenamebuf, c + 0x0020);
                break;
            case 0x0000: //  NULL
                appendChar(doctypenamebuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(doctypenamebuf, c);
                break;
            }
        }

        function after_doctype_name_state(c, lookahead, eof) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                /* Ignore the character. */
                nextchar += 1;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                nextchar += 1;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                tokenizer = data_state;
                break;
            default:
                lookahead = toUpperCase(lookahead);
                if (lookahead === "PUBLIC") {
                    nextchar += 6;
                    tokenizer = after_doctype_public_keyword_state;
                }
                else if (lookahead === "SYSTEM") {
                    nextchar += 6;
                    tokenizer = after_doctype_system_keyword_state;
                }
                else {
                    forcequirks();
                    tokenizer = bogus_doctype_state;
                }
                break;
            }
        }
        after_doctype_name_state.lookahead = 6;

        function after_doctype_public_keyword_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = before_doctype_public_identifier_state;
                break;
            case 0x0022: //  QUOTATION MARK
                beginDoctypePublicId();
                tokenizer = doctype_public_identifier_double_quoted_state;
                break;
            case 0x0027: //  APOSTROPHE
                beginDoctypePublicId();
                tokenizer = doctype_public_identifier_single_quoted_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                forcequirks();
                tokenizer = bogus_doctype_state;
                break;
            }
        }

        function before_doctype_public_identifier_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                /* Ignore the character. */
                break;
            case 0x0022: //  QUOTATION MARK
                beginDoctypePublicId();
                tokenizer = doctype_public_identifier_double_quoted_state;
                break;
            case 0x0027: //  APOSTROPHE
                beginDoctypePublicId();
                tokenizer = doctype_public_identifier_single_quoted_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                forcequirks();
                tokenizer = bogus_doctype_state;
                break;
            }
        }

        function doctype_public_identifier_double_quoted_state(c) {
            switch(c) {
            case 0x0022: //  QUOTATION MARK
                tokenizer = after_doctype_public_identifier_state;
                break;
            case 0x0000: //  NULL
                appendChar(doctypepublicbuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(doctypepublicbuf, c);
                break;
            }
        }

        function doctype_public_identifier_single_quoted_state(c) {
            switch(c) {
            case 0x0027: //  APOSTROPHE
                tokenizer = after_doctype_public_identifier_state;
                break;
            case 0x0000: //  NULL
                appendChar(doctypepublicbuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(doctypepublicbuf, c);
                break;
            }
        }

        function after_doctype_public_identifier_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = between_doctype_public_and_system_identifiers_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitDoctype();
                break;
            case 0x0022: //  QUOTATION MARK
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_double_quoted_state;
                break;
            case 0x0027: //  APOSTROPHE
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_single_quoted_state;
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                forcequirks();
                tokenizer = bogus_doctype_state;
                break;
            }
        }

        function between_doctype_public_and_system_identifiers_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE Ignore the character.
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitDoctype();
                break;
            case 0x0022: //  QUOTATION MARK
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_double_quoted_state;
                break;
            case 0x0027: //  APOSTROPHE
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_single_quoted_state;
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                forcequirks();
                tokenizer = bogus_doctype_state;
                break;
            }
        }

        function after_doctype_system_keyword_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                tokenizer = before_doctype_system_identifier_state;
                break;
            case 0x0022: //  QUOTATION MARK
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_double_quoted_state;
                break;
            case 0x0027: //  APOSTROPHE
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_single_quoted_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                forcequirks();
                tokenizer = bogus_doctype_state;
                break;
            }
        }

        function before_doctype_system_identifier_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE Ignore the character.
                break;
            case 0x0022: //  QUOTATION MARK
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_double_quoted_state;
                break;
            case 0x0027: //  APOSTROPHE
                beginDoctypeSystemId();
                tokenizer = doctype_system_identifier_single_quoted_state;
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                forcequirks();
                tokenizer = bogus_doctype_state;
                break;
            }
        }

        function doctype_system_identifier_double_quoted_state(c) {
            switch(c) {
            case 0x0022: //  QUOTATION MARK
                tokenizer = after_doctype_system_identifier_state;
                break;
            case 0x0000: //  NULL
                appendChar(doctypesystembuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(doctypesystembuf, c);
                break;
            }
        }

        function doctype_system_identifier_single_quoted_state(c) {
            switch(c) {
            case 0x0027: //  APOSTROPHE
                tokenizer = after_doctype_system_identifier_state;
                break;
            case 0x0000: //  NULL
                appendChar(doctypesystembuf, 0xFFFD /* REPLACEMENT CHARACTER */);
                break;
            case 0x003E: //  GREATER-THAN SIGN
                forcequirks();
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                appendChar(doctypesystembuf, c);
                break;
            }
        }

        function after_doctype_system_identifier_state(c) {
            switch(c) {
            case 0x0009: //  CHARACTER TABULATION (tab)
            case 0x000A: //  LINE FEED (LF)
            case 0x000C: //  FORM FEED (FF)
            case 0x0020: //  SPACE
                /* Ignore the character. */
                break;
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                forcequirks();
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                tokenizer = bogus_doctype_state;
                /* This does *not* set the DOCTYPE token's force-quirks flag. */
                break;
            }
        }

        function bogus_doctype_state(c) {
            switch(c) {
            case 0x003E: //  GREATER-THAN SIGN
                tokenizer = data_state;
                emitDoctype();
                break;
            case EOF:
                emitDoctype();
                nextchar--;  // pushback
                tokenizer = data_state;
                break;
            default:
                /* Ignore the character. */
                break;
            }
        }

        function cdata_section_state(c, lookahead, eof) {
            var len = lookahead.length;
            var output;
            if (eof) {
                nextchar += len-1; // leave the EOF in the scanner
                output = substring(lookahead,0, len-1); // don't include the EOF
            }
            else {
                nextchar += len;
                output = substring(lookahead,0,len-3); // don't emit the ]]>
            }

            if (output.length > 0) {
                if (output.indexOf("\u0000") !== -1)
                    textIncludesNUL = true;

                // XXX Have to deal with CR and CRLF here?
                if (output.indexOf("\r") !== -1) {
                    output = output.replace(/\r\n/, "\n").replace(/\r/, "\n");
                }

                emitCharString(output);
            }

            tokenizer = data_state;
        }
        cdata_section_state.lookahead = "]]>";


        /***
         * The tree builder insertion modes
         */

        // 11.2.5.4.1 The "initial" insertion mode
        function initial_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                value = replace(value,LEADINGWS, ""); // Ignore spaces
                if (value.length === 0) return; // Are we done?
                break;  // Handle anything non-space text below
            case COMMENT:
                doc.appendChild(doc.createComment(value));
                return;
            case DOCTYPE:
                var name = value;
                var publicid = arg3;
                var systemid = arg4;
                // Use the constructor directly instead of
                // implementation.createDocumentType because the create
                // function throws errors on invalid characters, and
                // we don't want the parser to throw them.
                doc.appendChild(new impl.DocumentType(name,publicid, systemid));

                // Note that there is no public API for setting quirks mode We can
                // do this here because we have access to implementation details
                if (force_quirks ||
                    toLowerCase(name) !== "html" ||
                    test(quirkyPublicIds, publicid) ||
                    (systemid && toLowerCase(systemid) === quirkySystemId) ||
                    (systemid === undefined &&
                     test(conditionallyQuirkyPublicIds, publicid)))
                    doc._quirks = true;
                else if (test(limitedQuirkyPublicIds, publicid) ||
                         (systemid !== undefined &&
                          test(conditionallyQuirkyPublicIds, publicid)))
                    doc._limitedQuirks = true;
                parser = before_html_mode;
                return;
            }

            // tags or non-whitespace text
            doc._quirks = true;
            parser = before_html_mode;
            parser(t,value,arg3,arg4);
        }

        // 11.2.5.4.2 The "before html" insertion mode
        function before_html_mode(t,value,arg3,arg4) {
            var elt;
            switch(t) {
            case TEXT:
                value = replace(value, LEADINGWS, ""); // Ignore spaces
                if (value.length === 0) return; // Are we done?
                break;  // Handle anything non-space text below
            case DOCTYPE:
                /* ignore the token */
                return;
            case COMMENT:
                doc.appendChild(doc.createComment(value));
                return;
            case TAG:
                if (value === "html") {
                    elt = createHTMLElt(value, arg3);
                    stack.push(elt);
                    doc.appendChild(elt);
                    // XXX: handle application cache here
                    parser = before_head_mode;
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "html":
                case "head":
                case "body":
                case "br":
                    break;   // fall through on these
                default:
                    return;  // ignore most end tags
                }
            }

            // Anything that didn't get handled above is handled like this:
            elt = createHTMLElt("html", null);
            stack.push(elt);
            doc.appendChild(elt);
            // XXX: handle application cache here
            parser = before_head_mode;
            parser(t,value,arg3,arg4);
        }

        // 11.2.5.4.3 The "before head" insertion mode
        function before_head_mode(t,value,arg3,arg4) {
            switch(t) {
            case TEXT:
                value = replace(value, LEADINGWS, "");  // Ignore spaces
                if (value.length === 0) return; // Are we done?
                break;  // Handle anything non-space text below
            case DOCTYPE:
                /* ignore the token */
                return;
            case COMMENT:
                insertComment(value);
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t,value,arg3,arg4);
                    return;
                case "head":
                    var elt = insertHTMLElement(value, arg3);
                    head_element_pointer = elt;
                    parser = in_head_mode;
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "html":
                case "head":
                case "body":
                case "br":
                    break;
                default:
                    return; // ignore most end tags
                }
            }

            // If not handled explicitly above
            before_head_mode(TAG, "head", null);  // create a head tag
            parser(t, value, arg3, arg4);  // then try again with this token
        }

        function in_head_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                var ws = match(value, LEADINGWS);
                if (ws) {
                    insertText(ws[0]);
                    value = substring(value,ws[0].length);
                }
                if (value.length === 0) return;
                break; // Handle non-whitespace below
            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "meta":
                    // XXX:
                    // May need to change the encoding based on this tag
                    /* falls through */
                case "base":
                case "basefont":
                case "bgsound":
                case "command":
                case "link":
                    insertHTMLElement(value, arg3);
                    stack.pop();
                    return;
                case "title":
                    parseRCDATA(value, arg3);
                    return;
                case "noscript":
                    if (!scripting_enabled) {
                        insertHTMLElement(value, arg3);
                        parser = in_head_noscript_mode;
                        return;
                    }
                    // Otherwise, if scripting is enabled...
                    /* falls through */
                case "noframes":
                case "style":
                    parseRawText(value,arg3);
                    return;
                case "script":
                    var elt = createHTMLElt(value, arg3);
                    elt._parser_inserted = true;
                    elt._force_async = false;
                    if (fragment) elt._already_started = true;
                    flushText();
                    stack.top.appendChild(elt);
                    stack.push(elt);
                    tokenizer = script_data_state;
                    originalInsertionMode = parser;
                    parser = text_mode;
                    return;
                case "head":
                    return; // ignore it
                }
                break;
            case ENDTAG:
                switch(value) {
                case "head":
                    stack.pop();
                    parser = after_head_mode;
                    return;
                case "body":
                case "html":
                case "br":
                    break; // handle these at the bottom of the function
                default:
                    // ignore any other end tag
                    return;
                }
                break;
            }

            // If not handled above
            in_head_mode(ENDTAG, "head", null);     // synthetic </head>
            parser(t, value, arg3, arg4);   // Then redo this one
        }

        // 13.2.5.4.5 The "in head noscript" insertion mode
        function in_head_noscript_mode(t, value, arg3, arg4) {
            switch(t) {
            case DOCTYPE:
                return;
            case COMMENT:
                in_head_mode(t, value);
                return;
            case TEXT:
                var ws = match(value, LEADINGWS);
                if (ws) {
                    in_head_mode(t, ws[0]);
                    value = substring(value,ws[0].length);
                }
                if (value.length === 0) return; // no more text
                break; // Handle non-whitespace below
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "basefont":
                case "bgsound":
                case "link":
                case "meta":
                case "noframes":
                case "style":
                    in_head_mode(t, value, arg3);
                    return;
                case "head":
                case "noscript":
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "noscript":
                    stack.pop();
                    parser = in_head_mode;
                    return;
                case "br":
                    break;  // goes to the outer default
                default:
                    return; // ignore other end tags
                }
                break;
            }

            // If not handled above
            in_head_noscript_mode(ENDTAG, "noscript", null);
            parser(t, value, arg3, arg4);
        }

        function after_head_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                var ws = match(value, LEADINGWS);
                if (ws) {
                    insertText(ws[0]);
                    value = substring(value,ws[0].length);
                }
                if (value.length === 0) return;
                break; // Handle non-whitespace below
            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "body":
                    insertHTMLElement(value, arg3);
                    frameset_ok = false;
                    parser = in_body_mode;
                    return;
                case "frameset":
                    insertHTMLElement(value, arg3);
                    parser = in_frameset_mode;
                    return;
                case "base":
                case "basefont":
                case "bgsound":
                case "link":
                case "meta":
                case "noframes":
                case "script":
                case "style":
                case "title":
                    stack.push(head_element_pointer);
                    in_head_mode(TAG, value, arg3);
                    stack.removeElement(head_element_pointer);
                    return;
                case "head":
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "body":
                case "html":
                case "br":
                    break;
                default:
                    return;  // ignore any other end tag
                }
                break;
            }

            after_head_mode(TAG, "body", null);
            frameset_ok = true;
            parser(t, value, arg3, arg4);
        }

        // 13.2.5.4.7 The "in body" insertion mode
        function in_body_mode(t,value,arg3,arg4) {
            var body, i, node;
            switch(t) {
            case TEXT:
                if (textIncludesNUL) {
                    value = replace(value, NULCHARS, "");
                    if (value.length === 0) return;
                }
                // If any non-space characters
                if (frameset_ok && test(NONWS, value))
                    frameset_ok = false;
                afereconstruct();
                insertText(value);
                return;
            case DOCTYPE:
                return;
            case COMMENT:
                insertComment(value);
                return;
            case EOF:
                stopParsing();
                return;
            case TAG:
                switch(value) {
                case "html":
                    transferAttributes(arg3, stack.elements[0]);
                    return;
                case "base":
                case "basefont":
                case "bgsound":
                case "command":
                case "link":
                case "meta":
                case "noframes":
                case "script":
                case "style":
                case "title":
                    in_head_mode(TAG, value, arg3);
                    return;
                case "body":
                    body = stack.elements[1];
                    if (!body || !(body instanceof impl.HTMLBodyElement))
                        return;
                    frameset_ok = false;
                    transferAttributes(arg3, body);
                    return;
                case "frameset":
                    if (!frameset_ok) return;
                    body = stack.elements[1];
                    if (!body || !(body instanceof impl.HTMLBodyElement))
                        return;
                    if (body.parentNode) body.parentNode.removeChild(body);
                    while(!(stack.top instanceof impl.HTMLHtmlElement))
                        stack.pop();
                    insertHTMLElement(value, arg3);
                    parser = in_frameset_mode;
                    return;

                case "address":
                case "article":
                case "aside":
                case "blockquote":
                case "center":
                case "details":
                case "dir":
                case "div":
                case "dl":
                case "fieldset":
                case "figcaption":
                case "figure":
                case "footer":
                case "header":
                case "hgroup":
                case "menu":
                case "nav":
                case "ol":
                case "p":
                case "section":
                case "summary":
                case "ul":
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    insertHTMLElement(value, arg3);
                    return;

                case "h1":
                case "h2":
                case "h3":
                case "h4":
                case "h5":
                case "h6":
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    if (stack.top instanceof impl.HTMLHeadingElement)
                        stack.pop();
                    insertHTMLElement(value, arg3);
                    return;

                case "pre":
                case "listing":
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    insertHTMLElement(value, arg3);
                    ignore_linefeed = true;
                    frameset_ok = false;
                    return;

                case "form":
                    if (form_element_pointer) return;
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    form_element_pointer = insertHTMLElement(value, arg3);
                    return;

                case "li":
                    frameset_ok = false;
                    for(i = stack.elements.length-1; i >= 0; i--) {
                        node = stack.elements[i];
                        if (node instanceof impl.HTMLLIElement) {
                            in_body_mode(ENDTAG, "li");
                            break;
                        }
                        if (isA(node, specialSet) && !isA(node, addressdivpSet))
                            break;
                    }
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    insertHTMLElement(value, arg3);
                    return;

                case "dd":
                case "dt":
                    frameset_ok = false;
                    for(i = stack.elements.length-1; i >= 0; i--) {
                        node = stack.elements[i];
                        if (isA(node, dddtSet)) {
                            in_body_mode(ENDTAG, node.localName);
                            break;
                        }
                        if (isA(node, specialSet) && !isA(node, addressdivpSet))
                            break;
                    }
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    insertHTMLElement(value, arg3);
                    return;

                case "plaintext":
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    insertHTMLElement(value, arg3);
                    tokenizer = plaintext_state;
                    return;

                case "button":
                    if (stack.inScope("button")) {
                        in_body_mode(ENDTAG, "button");
                        parser(t, value, arg3, arg4);
                    }
                    else {
                        afereconstruct();
                        insertHTMLElement(value, arg3);
                        frameset_ok = false;
                    }
                    return;

                case "a":
                    var activeElement = afe.findElementByTag("a");
                    if (activeElement) {
                        in_body_mode(ENDTAG, value);
                        afe.remove(activeElement);
                        stack.removeElement(activeElement);
                    }
                    /* falls through */

                case "b":
                case "big":
                case "code":
                case "em":
                case "font":
                case "i":
                case "s":
                case "small":
                case "strike":
                case "strong":
                case "tt":
                case "u":
                    afereconstruct();
                    afe.push(insertHTMLElement(value,arg3), arg3);
                    return;

                case "nobr":
                    afereconstruct();

                    if (stack.inScope(value)) {
                        in_body_mode(ENDTAG, value);
                        afereconstruct();
                    }
                    afe.push(insertHTMLElement(value,arg3), arg3);
                    return;

                case "applet":
                case "marquee":
                case "object":
                    afereconstruct();
                    insertHTMLElement(value,arg3);
                    afe.insertMarker();
                    frameset_ok = false;
                    return;

                case "table":
                    if (!doc._quirks && stack.inButtonScope("p")) {
                        in_body_mode(ENDTAG, "p");
                    }
                    insertHTMLElement(value,arg3);
                    frameset_ok = false;
                    parser = in_table_mode;
                    return;

                case "area":
                case "br":
                case "embed":
                case "img":
                case "keygen":
                case "wbr":
                    afereconstruct();
                    insertHTMLElement(value,arg3);
                    stack.pop();
                    frameset_ok = false;
                    return;

                case "input":
                    afereconstruct();
                    var elt = insertHTMLElement(value,arg3);
                    stack.pop();
                    var type = elt.getAttribute("type");
                    if (!type || toLowerCase(type) !== "hidden")
                        frameset_ok = false;
                    return;

                case "param":
                case "source":
                case "track":
                    insertHTMLElement(value,arg3);
                    stack.pop();
                    return;

                case "hr":
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    insertHTMLElement(value,arg3);
                    stack.pop();
                    frameset_ok = false;
                    return;

                case "image":
                    in_body_mode(TAG, "img", arg3, arg4);
                    return;

                case "isindex":
                    if (form_element_pointer) return;
                    (function handleIsIndexTag(attrs) {
                        var prompt = null;
                        var formattrs = [];
                        var newattrs = [["name", "isindex"]];
                        for(var i = 0; i < attrs.length; i++) {
                            var a = attrs[i];
                            if (a[0] === "action") {
                                push(formattrs, a);
                            }
                            else if (a[0] === "prompt") {
                                prompt = a[1];
                            }
                            else if (a[0] !== "name") {
                                push(newattrs, a);
                            }
                        }

                        // This default prompt presumably needs localization.
                        // The space after the colon in this prompt is required
                        // by the html5lib test cases
                        if (!prompt)
                            prompt = "This is a searchable index. " +
                            "Enter search keywords: ";

                        parser(TAG, "form", formattrs);
                        parser(TAG, "hr", null);
                        parser(TAG, "label", null);
                        parser(TEXT, prompt);
                        parser(TAG, "input", newattrs);
                        parser(ENDTAG, "label");
                        parser(TAG, "hr", null);
                        parser(ENDTAG, "form");
                    }(arg3));
                    return;

                case "textarea":
                    insertHTMLElement(value,arg3);
                    ignore_linefeed = true;
                    frameset_ok = false;
                    tokenizer = rcdata_state;
                    originalInsertionMode = parser;
                    parser = text_mode;
                    return;

                case "xmp":
                    if (stack.inButtonScope("p")) in_body_mode(ENDTAG, "p");
                    afereconstruct();
                    frameset_ok = false;
                    parseRawText(value, arg3);
                    return;

                case "iframe":
                    frameset_ok = false;
                    parseRawText(value, arg3);
                    return;

                case "noembed":
                    parseRawText(value,arg3);
                    return;

                case "noscript":
                    if (scripting_enabled) {
                        parseRawText(value,arg3);
                        return;
                    }
                    break;  // XXX Otherwise treat it as any other open tag?

                case "select":
                    afereconstruct();
                    insertHTMLElement(value,arg3);
                    frameset_ok = false;
                    if (parser === in_table_mode ||
                        parser === in_caption_mode ||
                        parser === in_table_body_mode ||
                        parser === in_row_mode ||
                        parser === in_cell_mode)
                        parser = in_select_in_table_mode;
                    else
                        parser = in_select_mode;
                    return;

                case "optgroup":
                case "option":
                    if (stack.top instanceof impl.HTMLOptionElement) {
                        in_body_mode(ENDTAG, "option");
                    }
                    afereconstruct();
                    insertHTMLElement(value,arg3);
                    return;

                case "rp":
                case "rt":
                    if (stack.inScope("ruby")) {
                        stack.generateImpliedEndTags();
                    }
                    insertHTMLElement(value,arg3);
                    return;

                case "math":
                    afereconstruct();
                    adjustMathMLAttributes(arg3);
                    adjustForeignAttributes(arg3);
                    insertForeignElement(value, arg3, MATHML_NAMESPACE);
                    if (arg4) // self-closing flag
                        stack.pop();
                    return;

                case "svg":
                    afereconstruct();
                    adjustSVGAttributes(arg3);
                    adjustForeignAttributes(arg3);
                    insertForeignElement(value, arg3, SVG_NAMESPACE);
                    if (arg4) // self-closing flag
                        stack.pop();
                    return;

                case "caption":
                case "col":
                case "colgroup":
                case "frame":
                case "head":
                case "tbody":
                case "td":
                case "tfoot":
                case "th":
                case "thead":
                case "tr":
                    // Ignore table tags if we're not in_table mode
                    return;
                }

                // Handle any other start tag here
                // (and also noscript tags when scripting is disabled)
                afereconstruct();
                insertHTMLElement(value,arg3);
                return;

            case ENDTAG:
                switch(value) {
                case "body":
                    if (!stack.inScope("body")) return;
                    parser = after_body_mode;
                    return;
                case "html":
                    if (!stack.inScope("body")) return;
                    parser = after_body_mode;
                    parser(t, value, arg3);
                    return;

                case "address":
                case "article":
                case "aside":
                case "blockquote":
                case "button":
                case "center":
                case "details":
                case "dir":
                case "div":
                case "dl":
                case "fieldset":
                case "figcaption":
                case "figure":
                case "footer":
                case "header":
                case "hgroup":
                case "listing":
                case "menu":
                case "nav":
                case "ol":
                case "pre":
                case "section":
                case "summary":
                case "ul":
                    // Ignore if there is not a matching open tag
                    if (!stack.inScope(value)) return;
                    stack.generateImpliedEndTags();
                    stack.popTag(value);
                    return;

                case "form":
                    var openform = form_element_pointer;
                    form_element_pointer = null;
                    if (!openform || !stack.elementInScope(openform)) return;
                    stack.generateImpliedEndTags();
                    stack.removeElement(openform);
                    return;

                case "p":
                    if (!stack.inButtonScope(value)) {
                        in_body_mode(TAG, value, null);
                        parser(t, value, arg3, arg4);
                    }
                    else {
                        stack.generateImpliedEndTags(value);
                        stack.popTag(value);
                    }
                    return;

                case "li":
                    if (!stack.inListItemScope(value)) return;
                    stack.generateImpliedEndTags(value);
                    stack.popTag(value);
                    return;

                case "dd":
                case "dt":
                    if (!stack.inScope(value)) return;
                    stack.generateImpliedEndTags(value);
                    stack.popTag(value);
                    return;

                case "h1":
                case "h2":
                case "h3":
                case "h4":
                case "h5":
                case "h6":
                    if (!stack.elementTypeInScope(impl.HTMLHeadingElement)) return;
                    stack.generateImpliedEndTags();
                    stack.popElementType(impl.HTMLHeadingElement);
                    return;

                case "a":
                case "b":
                case "big":
                case "code":
                case "em":
                case "font":
                case "i":
                case "nobr":
                case "s":
                case "small":
                case "strike":
                case "strong":
                case "tt":
                case "u":
                    var result = adoptionAgency(value);
                    if (result) return;  // If we did something we're done
                    break;               // Go to the "any other end tag" case

                case "applet":
                case "marquee":
                case "object":
                    if (!stack.inScope(value)) return;
                    stack.generateImpliedEndTags();
                    stack.popTag(value);
                    afe.clearToMarker();
                    return;

                case "br":
                    in_body_mode(TAG, value, null);  // Turn </br> into <br>
                    return;
                }

                // Any other end tag goes here
                for(i = stack.elements.length-1; i >= 0; i--) {
                    node = stack.elements[i];
                    if (node.localName === value) {
                        stack.generateImpliedEndTags(value);
                        stack.popElement(node);
                        break;
                    }
                    else if (isA(node, specialSet)) {
                        return;
                    }
                }

                return;
            }
        }

        function text_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                insertText(value);
                return;
            case EOF:
                if (stack.top instanceof impl.HTMLScriptElement)
                    stack.top._already_started = true;
                stack.pop();
                parser = originalInsertionMode;
                parser(t);
                return;
            case ENDTAG:
                if (value === "script") {
                    handleScriptEnd();
                }
                else {
                    stack.pop();
                    parser = originalInsertionMode;
                }
                return;
            default:
                // We should never get any other token types
                return;
            }
        }

        function in_table_mode(t, value, arg3, arg4) {
            function getTypeAttr(attrs) {
                for(var i = 0, n = attrs.length; i < n; i++) {
                    if (attrs[i][0] === "type")
                        return toLowerCase(attrs[i][1]);
                }
                return null;
            }

            switch(t) {
            case TEXT:
                // XXX the text_integration_mode stuff is
                // just a hack I made up
                if (text_integration_mode) {
                    in_body_mode(t, value, arg3, arg4)
                }
                else {
                    pending_table_text = [];
                    originalInsertionMode = parser;
                    parser = in_table_text_mode;
                    parser(t, value, arg3, arg4);
                }
                return;
            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                return;
            case TAG:
                switch(value) {
                case "caption":
                    stack.clearToContext(impl.HTMLTableElement);
                    afe.insertMarker();
                    insertHTMLElement(value,arg3);
                    parser = in_caption_mode;
                    return;
                case "colgroup":
                    stack.clearToContext(impl.HTMLTableElement);
                    insertHTMLElement(value,arg3);
                    parser = in_column_group_mode;
                    return;
                case "col":
                    in_table_mode(TAG, "colgroup", null);
                    parser(t, value, arg3, arg4);
                    return;
                case "tbody":
                case "tfoot":
                case "thead":
                    stack.clearToContext(impl.HTMLTableElement);
                    insertHTMLElement(value,arg3);
                    parser = in_table_body_mode;
                    return;
                case "td":
                case "th":
                case "tr":
                    in_table_mode(TAG, "tbody", null);
                    parser(t, value, arg3, arg4);
                    return;

                case "table":
                    var repro = stack.inTableScope(value);
                    in_table_mode(ENDTAG, value);
                    if (repro) parser(t, value, arg3, arg4);
                    return;

                case "style":
                case "script":
                    in_head_mode(t, value, arg3, arg4);
                    return;

                case "input":
                    var type = getTypeAttr(arg3);
                    if (type !== "hidden") break;  // to the anything else case
                    insertHTMLElement(value,arg3);
                    stack.pop();
                    return;

                case "form":
                    if (form_element_pointer) return;
                    form_element_pointer = insertHTMLElement(value, arg3);
                    stack.pop();
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "table":
                    if (!stack.inTableScope(value)) return;
                    stack.popTag(value);
                    resetInsertionMode();
                    return;
                case "body":
                case "caption":
                case "col":
                case "colgroup":
                case "html":
                case "tbody":
                case "td":
                case "tfoot":
                case "th":
                case "thead":
                case "tr":
                    return;
                }

                break;
            case EOF:
                stopParsing();
                return;
            }

            // This is the anything else case
            foster_parent_mode = true;
            in_body_mode(t, value, arg3, arg4);
            foster_parent_mode = false;
        }

        function in_table_text_mode(t, value, arg3, arg4) {
            if (t === TEXT) {
                if (textIncludesNUL) {
                    value = replace(value, NULCHARS, "");
                    if (value.length === 0) return;
                }
                push(pending_table_text,value);
            }
            else {
                var s = join(pending_table_text,"");
                pending_table_text.length = 0;
                if (test(NONWS, s)) { // If any non-whitespace characters
                    // This must be the same code as the "anything else"
                    // case of the in_table mode above.
                    foster_parent_mode = true;
                    in_body_mode(TEXT, s);
                    foster_parent_mode = false;
                }
                else {
                    insertText(s);
                }
                parser = originalInsertionMode;
                parser(t, value, arg3, arg4);
            }
        }


        function in_caption_mode(t, value, arg3, arg4) {
            function end_caption() {
                if (!stack.inTableScope("caption")) return false;
                stack.generateImpliedEndTags();
                stack.popTag("caption");
                afe.clearToMarker();
                parser = in_table_mode;
                return true;
            }

            switch(t) {
            case TAG:
                switch(value) {
                case "caption":
                case "col":
                case "colgroup":
                case "tbody":
                case "td":
                case "tfoot":
                case "th":
                case "thead":
                case "tr":
                    if (end_caption()) parser(t, value, arg3, arg4);
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "caption":
                    end_caption();
                    return;
                case "table":
                    if (end_caption()) parser(t, value, arg3, arg4);
                    return;
                case "body":
                case "col":
                case "colgroup":
                case "html":
                case "tbody":
                case "td":
                case "tfoot":
                case "th":
                case "thead":
                case "tr":
                    return;
                }
                break;
            }

            // The Anything Else case
            in_body_mode(t, value, arg3, arg4);
        }

        function in_column_group_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                var ws = match(value, LEADINGWS);
                if (ws) {
                    insertText(ws[0]);
                    value = substring(value,ws[0].length);
                }
                if (value.length === 0) return;
                break; // Handle non-whitespace below

            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "col":
                    insertHTMLElement(value, arg3);
                    stack.pop();
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "colgroup":
                    if (stack.top instanceof impl.HTMLHtmlElement) return;
                    stack.pop();
                    parser = in_table_mode;
                    return;
                case "col":
                    return;
                }
                break;
            case EOF:
                if (stack.top instanceof impl.HTMLHtmlElement) {
                    stopParsing();
                    return;
                }
                break;
            }

            // Anything else
            if (!(stack.top instanceof impl.HTMLHtmlElement)) {
                in_column_group_mode(ENDTAG, "colgroup");
                parser(t, value, arg3, arg4);
            }
        }

        function in_table_body_mode(t, value, arg3, arg4) {
            function endsect() {
                if (!stack.inTableScope("tbody") &&
                    !stack.inTableScope("thead") &&
                    !stack.inTableScope("tfoot"))
                    return;
                stack.clearToContext(impl.HTMLTableSectionElement);
                in_table_body_mode(ENDTAG, stack.top.localName, null);
                parser(t, value, arg3, arg4);
            }

            switch(t) {
            case TAG:
                switch(value) {
                case "tr":
                    stack.clearToContext(impl.HTMLTableSectionElement);
                    insertHTMLElement(value, arg3);
                    parser = in_row_mode;
                    return;
                case "th":
                case "td":
                    in_table_body_mode(TAG, "tr", null);
                    parser(t, value, arg3, arg4);
                    return;
                case "caption":
                case "col":
                case "colgroup":
                case "tbody":
                case "tfoot":
                case "thead":
                    endsect();
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "table":
                    endsect();
                    return;
                case "tbody":
                case "tfoot":
                case "thead":
                    if (stack.inTableScope(value)) {
                        stack.clearToContext(impl.HTMLTableSectionElement);
                        stack.pop();
                        parser = in_table_mode;
                    }
                    return;
                case "body":
                case "caption":
                case "col":
                case "colgroup":
                case "html":
                case "td":
                case "th":
                case "tr":
                    return;
                }
                break;
            }

            // Anything else:
            in_table_mode(t, value, arg3, arg4);
        }

        function in_row_mode(t, value, arg3, arg4) {
            function endrow() {
                if (!stack.inTableScope("tr")) return false;
                stack.clearToContext(impl.HTMLTableRowElement);
                stack.pop();
                parser = in_table_body_mode;
                return true;
            }

            switch(t) {
            case TAG:
                switch(value) {
                case "th":
                case "td":
                    stack.clearToContext(impl.HTMLTableRowElement);
                    insertHTMLElement(value, arg3);
                    parser = in_cell_mode;
                    afe.insertMarker();
                    return;
                case "caption":
                case "col":
                case "colgroup":
                case "tbody":
                case "tfoot":
                case "thead":
                case "tr":
                    if (endrow()) parser(t, value, arg3, arg4);
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "tr":
                    endrow();
                    return;
                case "table":
                    if (endrow()) parser(t, value, arg3, arg4);
                    return;
                case "tbody":
                case "tfoot":
                case "thead":
                    if (stack.inTableScope(value)) {
                        in_row_mode(ENDTAG, "tr");
                        parser(t, value, arg3, arg4);
                    }
                    return;
                case "body":
                case "caption":
                case "col":
                case "colgroup":
                case "html":
                case "td":
                case "th":
                    return;
                }
                break;
            }

            // anything else
            in_table_mode(t, value, arg3, arg4);
        }

        function in_cell_mode(t, value, arg3, arg4) {
            switch(t) {
            case TAG:
                switch(value) {
                case "caption":
                case "col":
                case "colgroup":
                case "tbody":
                case "td":
                case "tfoot":
                case "th":
                case "thead":
                case "tr":
                    if (stack.inTableScope("td")) {
                        in_cell_mode(ENDTAG, "td");
                        parser(t, value, arg3, arg4);
                    }
                    else if (stack.inTableScope("th")) {
                        in_cell_mode(ENDTAG, "th");
                        parser(t, value, arg3, arg4);
                    }
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "td":
                case "th":
                    if (!stack.inTableScope(value)) return;
                    stack.generateImpliedEndTags();
                    stack.popTag(value);
                    afe.clearToMarker();
                    parser = in_row_mode;
                    return;

                case "body":
                case "caption":
                case "col":
                case "colgroup":
                case "html":
                    return;

                case "table":
                case "tbody":
                case "tfoot":
                case "thead":
                case "tr":
                    if (!stack.inTableScope(value)) return;
                    in_cell_mode(ENDTAG, stack.inTableScope("td") ? "td" : "th");
                    parser(t, value, arg3, arg4);
                    return;
                }
                break;
            }

            // anything else
            in_body_mode(t, value, arg3, arg4);
        }

        function in_select_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                if (textIncludesNUL) {
                    value = replace(value, NULCHARS, "");
                    if (value.length === 0) return;
                }
                insertText(value);
                return;
            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                return;
            case EOF:
                stopParsing();
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "option":
                    if (stack.top instanceof impl.HTMLOptionElement)
                        in_select_mode(ENDTAG, value);
                    insertHTMLElement(value, arg3);
                    return;
                case "optgroup":
                    if (stack.top instanceof impl.HTMLOptionElement)
                        in_select_mode(ENDTAG, "option");
                    if (stack.top instanceof impl.HTMLOptGroupElement)
                        in_select_mode(ENDTAG, value);
                    insertHTMLElement(value, arg3);
                    return;
                case "select":
                    in_select_mode(ENDTAG, value); // treat it as a close tag
                    return;

                case "input":
                case "keygen":
                case "textarea":
                    if (!stack.inSelectScope("select")) return;
                    in_select_mode(ENDTAG, "select");
                    parser(t, value, arg3, arg4);
                    return;

                case "script":
                    in_head_mode(t, value, arg3, arg4);
                    return;
                }
                break;
            case ENDTAG:
                switch(value) {
                case "optgroup":
                    if (stack.top instanceof impl.HTMLOptionElement &&
                        stack.elements[stack.elements.length-2] instanceof
                        impl.HTMLOptGroupElement) {
                        in_select_mode(ENDTAG, "option");
                    }
                    if (stack.top instanceof impl.HTMLOptGroupElement)
                        stack.pop();

                    return;

                case "option":
                    if (stack.top instanceof impl.HTMLOptionElement)
                        stack.pop();
                    return;

                case "select":
                    if (!stack.inSelectScope(value)) return;
                    stack.popTag(value);
                    resetInsertionMode();
                    return;
                }

                break;
            }

            // anything else: just ignore the token
        }

        function in_select_in_table_mode(t, value, arg3, arg4) {
            switch(value) {
            case "caption":
            case "table":
            case "tbody":
            case "tfoot":
            case "thead":
            case "tr":
            case "td":
            case "th":
                switch(t) {
                case TAG:
                    in_select_in_table_mode(ENDTAG, "select");
                    parser(t, value, arg3, arg4);
                    return;
                case ENDTAG:
                    if (stack.inTableScope(value)) {
                        in_select_in_table_mode(ENDTAG, "select");
                        parser(t, value, arg3, arg4);
                    }
                    return;
                }
            }

            // anything else
            in_select_mode(t, value, arg3, arg4);
        }

        function after_body_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                // If any non-space chars, handle below
                if (test(NONWS, value)) break;
                in_body_mode(t, value);
                return;
            case COMMENT:
                // Append it to the <html> element
                stack.elements[0].appendChild(doc.createComment(value));
                return;
            case DOCTYPE:
                return;
            case EOF:
                stopParsing();
                return;
            case TAG:
                if (value === "html") {
                    in_body_mode(t, value, arg3, arg4);
                    return;
                }
                break; // for any other tags
            case ENDTAG:
                if (value === "html") {
                    if (fragment) return;
                    parser = after_after_body_mode;
                    return;
                }
                break; // for any other tags
            }

            // anything else
            parser = in_body_mode;
            parser(t, value, arg3, arg4);
        }

        function in_frameset_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                // Ignore any non-space characters
                value = replace(value, ALLNONWS, "");
                if (value.length > 0) insertText(value);
                return;
            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                return;
            case EOF:
                stopParsing();
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "frameset":
                    insertHTMLElement(value, arg3);
                    return;
                case "frame":
                    insertHTMLElement(value, arg3);
                    stack.pop();
                    return;
                case "noframes":
                    in_head_mode(t, value, arg3, arg4);
                    return;
                }
                break;
            case ENDTAG:
                if (value === "frameset") {
                    if (fragment && stack.top instanceof impl.HTMLHtmlElement)
                        return;
                    stack.pop();
                    if (!fragment &&
                        !(stack.top instanceof impl.HTMLFrameSetElement))
                        parser = after_frameset_mode;
                    return;
                }
                break;
            }

            // ignore anything else
        }

        function after_frameset_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                // Ignore any non-space characters
                value = replace(value, ALLNONWS, "");
                if (value.length > 0) insertText(value);
                return;
            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                return;
            case EOF:
                stopParsing();
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "noframes":
                    in_head_mode(t, value, arg3, arg4);
                    return;
                }
                break;
            case ENDTAG:
                if (value === "html") {
                    parser = after_after_frameset_mode;
                    return;
                }
                break;
            }

            // ignore anything else
        }

        function after_after_body_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                // If any non-space chars, handle below
                if (test(NONWS, value)) break;
                in_body_mode(t, value, arg3, arg4);
                return;
            case COMMENT:
                doc.appendChild(doc.createComment(value));
                return;
            case DOCTYPE:
                in_body_mode(t, value, arg3, arg4);
                return;
            case EOF:
                stopParsing();
                return;
            case TAG:
                if (value === "html") {
                    in_body_mode(t, value, arg3, arg4);
                    return;
                }
                break;
            }

            // anything else
            parser = in_body_mode;
            parser(t, value, arg3, arg4);
        }

        function after_after_frameset_mode(t, value, arg3, arg4) {
            switch(t) {
            case TEXT:
                // Ignore any non-space characters
                value = replace(value, ALLNONWS, "");
                if (value.length > 0)
                    in_body_mode(t, value, arg3, arg4);
                return;
            case COMMENT:
                doc.appendChild(doc.createComment(value));
                return;
            case DOCTYPE:
                in_body_mode(t, value, arg3, arg4);
                return;
            case EOF:
                stopParsing();
                return;
            case TAG:
                switch(value) {
                case "html":
                    in_body_mode(t, value, arg3, arg4);
                    return;
                case "noframes":
                    in_head_mode(t, value, arg3, arg4);
                    return;
                }
                break;
            }

            // ignore anything else
        }


        // 13.2.5.5 The rules for parsing tokens in foreign content
        //
        // This is like one of the insertion modes above, but is
        // invoked somewhat differently when the current token is not HTML.
        // See the insertToken() function.
        function insertForeignToken(t, value, arg3, arg4) {
            // A <font> tag is an HTML font tag if it has a color, font, or size
            // attribute.  Otherwise we assume it is foreign content
            function isHTMLFont(attrs) {
                for(var i = 0, n = attrs.length; i < n; i++) {
                    switch(attrs[i][0]) {
                    case "color":
                    case "font":
                    case "size":
                        return true;
                    }
                }
                return false;
            }

            var current;

            switch(t) {
            case TEXT:
                // If any non-space, non-nul characters
                if (frameset_ok && test(NONWSNONNUL, value))
                    frameset_ok = false;
                if (textIncludesNUL) {
                    value = replace(value, NULCHARS, "\uFFFD");
                }
                insertText(value);
                return;
            case COMMENT:
                insertComment(value);
                return;
            case DOCTYPE:
                // ignore it
                return;
            case TAG:
                switch(value) {
                case "font":
                    if (!isHTMLFont(arg3)) break;
                    /* falls through */
                case "b":
                case "big":
                case "blockquote":
                case "body":
                case "br":
                case "center":
                case "code":
                case "dd":
                case "div":
                case "dl":
                case "dt":
                case "em":
                case "embed":
                case "h1":
                case "h2":
                case "h3":
                case "h4":
                case "h5":
                case "h6":
                case "head":
                case "hr":
                case "i":
                case "img":
                case "li":
                case "listing":
                case "menu":
                case "meta":
                case "nobr":
                case "ol":
                case "p":
                case "pre":
                case "ruby":
                case "s":
                case "small":
                case "span":
                case "strong":
                case "strike":
                case "sub":
                case "sup":
                case "table":
                case "tt":
                case "u":
                case "ul":
                case "var":
                    do {
                        stack.pop();
                        current = stack.top;
                    } while(current.namespaceURI !== HTML_NAMESPACE &&
                            !isMathmlTextIntegrationPoint(current) &&
                            !isHTMLIntegrationPoint(current));

                    insertToken(t, value, arg3, arg4);  // reprocess
                    return;
                }

                // Any other start tag case goes here
                current = stack.top;
                if (current.namespaceURI === MATHML_NAMESPACE) {
                    adjustMathMLAttributes(arg3);
                }
                else if (current.namespaceURI === SVG_NAMESPACE) {
                    value = adjustSVGTagName(value);
                    adjustSVGAttributes(arg3);
                }
                adjustForeignAttributes(arg3)

                insertForeignElement(value, arg3, current.namespaceURI);
                if (arg4) // the self-closing flag
                    stack.pop();
                return;

            case ENDTAG:
                current = stack.top;
                if (value === "script" &&
                    current.namespaceURI === SVG_NAMESPACE &&
                    current.localName === "script") {

                    stack.pop();

                    // XXX
                    // Deal with SVG scripts here
                }
                else {
                    // The any other end tag case
                    var i = stack.elements.length-1;
                    var node = stack.elements[i];
                    for(;;) {
                        if (toLowerCase(node.localName) === value) {
                            stack.popElement(node);
                            break;
                        }
                        node = stack.elements[--i];
                        // If non-html, keep looping
                        if (node.namespaceURI !== HTML_NAMESPACE)
                            continue;
                        // Otherwise process the end tag as html
                        parser(t, value, arg3, arg4);
                        break;
                    }
                }
                return;
            }
        }


        /***
         * parsing code for character references
         */

        // Parse a character reference from s and return a codepoint or an
        // array of codepoints or null if there is no valid char ref in s.
        function parseCharRef(s, isattr) {
            var len = s.length;
            var rv;
            if (len === 0) return null;  // No character reference matched

            if (s[0] === "#") {               // Numeric character reference
                var codepoint;

                if (s[1] === "x" || s[1] === "X") {
                    // Hex
                    codepoint = parseInt(substring(s,2), 16);
                }
                else {
                    // Decimal
                    codepoint = parseInt(substring(s,1), 10);
                }

                if (s[len-1] === ";")  // If the string ends with a semicolon
                    nextchar += len;   // Consume all the chars
                else
                    nextchar += len-1; // Otherwise, all but the last character

                if (codepoint in numericCharRefReplacements) {
                    codepoint = numericCharRefReplacements[codepoint];
                }
                else if (codepoint > 0x10FFFF ||
                         (codepoint >= 0xD800 && codepoint < 0xE000)) {
                    codepoint = 0xFFFD;
                }

                if (codepoint <= 0xFFFF) return codepoint;

                codepoint = codepoint - 0x10000;
                return [0xD800 + (codepoint >> 10),
                        0xDC00 + (codepoint & 0x03FF)];
            }
            else {                           // Named character reference
                // We have to be able to parse some named char refs even when
                // the semicolon is omitted, but have to match the longest one
                // possible.  So if the lookahead doesn't end with semicolon
                // then we have to loop backward looking for longest to shortest
                // matches.  Fortunately, the names that don't require semis
                // are all between 2 and 6 characters long.

                if (s[len-1] === ";") {
                    rv = namedCharRefs[s];
                    if (rv !== undefined) {
                        nextchar += len;  // consume all the characters
                        return rv;
                    }
                }

                // If it didn't end with a semicolon, see if we can match
                // everything but the terminating character
                len--;  // Ignore whatever the terminating character is
                rv = namedCharRefsNoSemi[substring(s,0, len)];
                if (rv !== undefined) {
                    nextchar += len;
                    return rv;
                }

                // If it still didn't match, and we're not parsing a
                // character reference in an attribute value, then try
                // matching shorter substrings.
                if (!isattr) {
                    len--;
                    if (len > 6) len = 6; // Maximum possible match length
                    while(len >= 2) {
                        rv = namedCharRefsNoSemi[substring(s,0, len)];
                        if (rv !== undefined) {
                            nextchar += len;
                            return rv;
                        }
                        len--;
                    }
                }

                // Couldn't find any match
                return null;
            }
        }


        /***
         * Finally, this is the end of the HTMLParser() factory function.
         * It returns the htmlparser object with the append() and end() methods.
         */

        // Sneak another method into the htmlparser object to allow us to run
        // tokenizer tests.  This can be commented out in production code.
        // This is a hook for testing the tokenizer. It has to be here
        // because the tokenizer details are all hidden away within the closure.
        // It should return an array of tokens generated while parsing the
        // input string.
        htmlparser.testTokenizer = function(input, initialState,
                                            lastStartTag, charbychar)
        {
            var tokens = [];

            switch(initialState) {
            case "PCDATA state":
                tokenizer = data_state;
                break;
            case "RCDATA state":
                tokenizer = rcdata_state;
                break;
            case "RAWTEXT state":
                tokenizer = rawtext_state;
                break;
            case "PLAINTEXT state":
                tokenizer = plaintext_state;
                break;
            }

            if (lastStartTag) {
                lasttagname = lastStartTag;
            }

            insertToken = function(t, value, arg3, arg4) {
                flushText();
                switch(t) {
                case TEXT:
                    if (tokens.length > 0 &&
                        tokens[tokens.length-1][0] === "Character") {
                        tokens[tokens.length-1][1] += value;
                    }
                    else push(tokens, ["Character", value]);
                    break;
                case COMMENT:
                    push(tokens,["Comment", value]);
                    break;
                case DOCTYPE:
                    push(tokens,["DOCTYPE", value,
                                 arg3 === undefined ? null : arg3,
                                 arg4 === undefined ? null : arg4,
                                 !force_quirks]);
                    break;
                case TAG:
                    var attrs = {};
                    for(var i = 0; i < arg3.length; i++) {
                        // XXX: does attribute order matter?
                        var a = arg3[i];
                        if (a.length === 1) {
                            attrs[a[0]] = "";
                        }
                        else {
                            attrs[a[0]] = a[1];
                        }
                    }
                    var token = ["StartTag", value, attrs];
                    if (arg4) push(token, true);
                    push(tokens,token);
                    break;
                case ENDTAG:
                    push(tokens,["EndTag", value]);
                    break;
                case EOF:
                    break;
                }
            }

            if (!charbychar) {
                this.parse(input, true);
            }
            else {
                for(var i = 0; i < input.length; i++) {
                    this.parse(input[i]);
                }
                this.parse("", true);
            }
            return tokens;
        };

        // Return the parser object from the HTMLParser() factory function
        return htmlparser;
    }

    // Return the HTMLParser factory function from the containing
    // closure that holds all the details.
    return HTMLParser;
}());


/************************************************************************
 *  src/impl/CSSStyleDeclaration.js
 ************************************************************************/

//@line 1 "src/impl/CSSStyleDeclaration.js"
defineLazyProperty(impl, "CSSStyleDeclaration", function() {
    function CSSStyleDeclaration(elt) {
        this._element = elt;
    }

    // Utility function for parsing style declarations
    // Pass in a string like "margin-left: 5px; border-style: solid"
    // and this function returns an object like
    // {"margin-left":"5px", "border-style":"solid"}
    function parseStyles(s) {
        var parser = new parserlib.css.Parser();
        var result = {};
        parser.addListener("property", function(e) {
            if (e.invalid) return;  // Skip errors
            result[e.property.text] = e.value.text;
            if (e.important) result.important[name] = e.important;
        });
        parser.parseStyleAttribute(s);
        return result;
    }

    CSSStyleDeclaration.prototype = O.create(Object.prototype, {
        _idlName: constant("CSSStyleDeclaration"),

        // Return the parsed form of the element's style attribute.
        // If the element's style attribute has never been parsed
        // or if it has changed since the last parse, then reparse it
        // Note that the styles don't get parsed until they're actually needed
        _parsed: attribute(function() {
            if (!this._parsedStyles || this.cssText !== this._lastParsedText) {
                var text = this.cssText;
                this._parsedStyles = parseStyles(text);
                this._lastParsedText = text;
                delete this._names;
            }
            return this._parsedStyles;
        }),

        // Call this method any time the parsed representation of the
        // style changes.  It converts the style properties to a string and
        // sets cssText and the element's style attribute
        _serialize: constant(function() {
            var styles = this._parsed;
            var s = "";

            for(var name in styles) {
                if (s) s += "; ";
                s += name + ":" + styles[name]
            }

            this.cssText = s;          // also sets the style attribute
            this._lastParsedText = s;  // so we don't reparse
            delete this._names;
        }),

        cssText: attribute(
            function() {
                // XXX: this is a CSSStyleDeclaration for an element.
                // A different impl might be necessary for a set of styles
                // associated returned by getComputedStyle(), e.g.
                return this._element.getAttribute("style");
            },
            function(value) {
                // XXX: I should parse and serialize the value to
                // normalize it and remove errors. FF and chrome do that.
                this._element.setAttribute("style", value);
            }
        ),

        length: attribute(function() {
            if (!this._names)
                this._names = Object.getOwnPropertyNames(this._parsed);
            return this._names.length;
        }),

        item: constant(function(n) {
            if (!this._names)
                this._names = Object.getOwnPropertyNames(this._parsed);
            return this._names[n];
        }),

        getPropertyValue: constant(function(property) {
            return this._parsed[toLowerCase(property)];
        }),

        // XXX: for now we ignore !important declarations
        getPropertyPriority: constant(function(property) {
            return "";
        }),

        // XXX the priority argument is ignored for now
        setProperty: constant(function(property, value, priority) {
            property = toLowerCase(property);
            if (value === "") {
                this.removeProperty(property);
                return;
            }

            // XXX are there other legal priority values?
            if (priority !== undefined && priority !== "important")
                return;

            // We don't just accept the property value.  Instead
            // we parse it to ensure that it is something valid.
            // If it contains a semicolon it is invalid
            if (value.indexOf(";") !== -1) return;

            var props = parseStyles(property + ":" + value);
            var newvalue = props[property];
            // If there is no value now, it wasn't valid
            if (!newvalue) return;

            var styles = this._parsed;

            // If the value didn't change, return without doing anything.
            var oldvalue = styles[property];
            if (newvalue === oldvalue) return;

            styles[property] = value;

            // Serialize and update cssText and element.style!
            this._serialize();
        }),

        removeProperty: constant(function(property) {
            property = toLowerCase(property);
            var styles = this._parsed;
            if (property in styles) {
                delete styles[property];

                // Serialize and update cssText and element.style!
                this._serialize();
            }
        }),
    });

    var cssProperties = {
        background: "background",
        backgroundAttachment: "background-attachment",
        backgroundColor: "background-color",
        backgroundImage: "background-image",
        backgroundPosition: "background-position",
        backgroundRepeat: "background-repeat",
        border: "border",
        borderCollapse: "border-collapse",
        borderColor: "border-color",
        borderSpacing: "border-spacing",
        borderStyle: "border-style",
        borderTop: "border-top",
        borderRight: "border-right",
        borderBottom: "border-bottom",
        borderLeft: "border-left",
        borderTopColor: "border-top-color",
        borderRightColor: "border-right-color",
        borderBottomColor: "border-bottom-color",
        borderLeftColor: "border-left-color",
        borderTopStyle:	"border-top-style",
        borderRightStyle: "border-right-style",
        borderBottomStyle: "border-bottom-style",
        borderLeftStyle: "border-left-style",
        borderTopWidth: "border-top-width",
        borderRightWidth: "border-right-width",
        borderBottomWidth: "border-bottom-width",
        borderLeftWidth: "border-left-width",
        borderWidth: "border-width",
        bottom: "bottom",
        captionSide: "caption-side",
        clear: "clear",
        clip: "clip",
        color: "color",
        content: "content",
        counterIncrement: "counter-increment",
        counterReset: "counter-reset",
        cursor: "cursor",
        direction: "direction",
        display: "display",
        emptyCells: "empty-cells",
        cssFloat: "float",
        font: "font",
        fontFamily: "font-family",
        fontSize: "font-size",
        fontSizeAdjust: "font-size-adjust",
        fontStretch: "font-stretch",
        fontStyle: "font-style",
        fontVariant: "font-variant",
        fontWeight: "font-weight",
        height: "height",
        left: "left",
        letterSpacing: "letter-spacing",
        lineHeight: "line-height",
        listStyle: "list-style",
        listStyleImage: "list-style-image",
        listStylePosition: "list-style-position",
        listStyleType: "list-style-type",
        margin: "margin",
        marginTop: "margin-top",
        marginRight: "margin-right",
        marginBottom: "margin-bottom",
        marginLeft: "margin-left",
        markerOffset: "marker-offset",
        marks: "marks",
        maxHeight: "max-height",
        maxWidth: "max-width",
        minHeight: "min-height",
        minWidth: "min-width",
        opacity: "opacity",
        orphans: "orphans",
        outline: "outline",
        outlineColor: "outline-color",
        outlineStyle: "outline-style",
        outlineWidth: "outline-width",
        overflow: "overflow",
        padding: "padding",
        paddingTop: "padding-top",
        paddingRight: "padding-right",
        paddingBottom: "padding-bottom",
        paddingLeft: "padding-left",
        page: "page",
        pageBreakAfter: "page-break-after",
        pageBreakBefore: "page-break-before",
        pageBreakInside: "page-break-inside",
        position: "position",
        quotes: "quotes",
        right: "right",
        size: "size",
        tableLayout: "table-layout",
        textAlign: "text-align",
        textDecoration: "text-decoration",
        textIndent: "text-indent",
        textShadow: "text-shadow",
        textTransform: "text-transform",
        top: "top",
        unicodeBidi: "unicode-bidi",
        verticalAlign: "vertical-align",
        visibility: "visibility",
        whiteSpace: "white-space",
        widows: "widows",
        width: "width",
        wordSpacing: "word-spacing",
        zIndex: "z-index",
    };

    for(var prop in cssProperties) defineStyleProperty(prop);

    function defineStyleProperty(jsname) {
        var cssname = cssProperties[jsname];
        Object.defineProperty(CSSStyleDeclaration.prototype, jsname, {
            get: function() {
                return this.getPropertyValue(cssname);
            },
            set: function(value) {
                if (value === "" || value == null) { // XXX?
                    this.removeProperty(cssname);
                }
                else {
                    // XXX Handle important declarations here!
                    this.setProperty(cssname, value);
                }
            }
        });
    }

    return CSSStyleDeclaration;
});


/************************************************************************
 *  src/impl/cssparser.js
 ************************************************************************/

//@line 1 "src/impl/cssparser.js"
/*!
Parser-Lib
Copyright (c) 2009-2011 Nicholas C. Zakas. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
/* Build time: 21-November-2017 09:22:26 */
var parserlib = {};
(function(){

/**
 * A generic base to inherit from for any object
 * that needs event handling.
 * @class EventTarget
 * @constructor
 */
function EventTarget(){

    /**
     * The array of listeners for various events.
     * @type Object
     * @property _listeners
     * @private
     */
    this._listeners = {};    
}

EventTarget.prototype = {

    //restore constructor
    constructor: EventTarget,

    /**
     * Adds a listener for a given event type.
     * @param {String} type The type of event to add a listener for.
     * @param {Function} listener The function to call when the event occurs.
     * @return {void}
     * @method addListener
     */
    addListener: function(type, listener){
        if (!this._listeners[type]){
            this._listeners[type] = [];
        }

        this._listeners[type].push(listener);
    },
    
    /**
     * Fires an event based on the passed-in object.
     * @param {Object|String} event An object with at least a 'type' attribute
     *      or a string indicating the event name.
     * @return {void}
     * @method fire
     */    
    fire: function(event){
        if (typeof event == "string"){
            event = { type: event };
        }
        if (typeof event.target != "undefined"){
            event.target = this;
        }
        
        if (typeof event.type == "undefined"){
            throw new Error("Event object missing 'type' property.");
        }
        
        if (this._listeners[event.type]){
        
            //create a copy of the array and use that so listeners can't chane
            var listeners = this._listeners[event.type].concat();
            for (var i=0, len=listeners.length; i < len; i++){
                listeners[i].call(this, event);
            }
        }            
    },

    /**
     * Removes a listener for a given event type.
     * @param {String} type The type of event to remove a listener from.
     * @param {Function} listener The function to remove from the event.
     * @return {void}
     * @method removeListener
     */
    removeListener: function(type, listener){
        if (this._listeners[type]){
            var listeners = this._listeners[type];
            for (var i=0, len=listeners.length; i < len; i++){
                if (listeners[i] === listener){
                    listeners.splice(i, 1);
                    break;
                }
            }
            
            
        }            
    }
};
/**
 * Convenient way to read through strings.
 * @namespace parserlib.util
 * @class StringReader
 * @constructor
 * @param {String} text The text to read.
 */
function StringReader(text){

    /**
     * The input text with line endings normalized.
     * @property _input
     * @type String
     * @private
     */
    this._input = text.replace(/\n\r?/g, "\n");


    /**
     * The row for the character to be read next.
     * @property _line
     * @type int
     * @private
     */
    this._line = 1;


    /**
     * The column for the character to be read next.
     * @property _col
     * @type int
     * @private
     */
    this._col = 1;

    /**
     * The index of the character in the input to be read next.
     * @property _cursor
     * @type int
     * @private
     */
    this._cursor = 0;
}

StringReader.prototype = {

    //restore constructor
    constructor: StringReader,

    //-------------------------------------------------------------------------
    // Position info
    //-------------------------------------------------------------------------

    /**
     * Returns the column of the character to be read next.
     * @return {int} The column of the character to be read next.
     * @method getCol
     */
    getCol: function(){
        return this._col;
    },

    /**
     * Returns the row of the character to be read next.
     * @return {int} The row of the character to be read next.
     * @method getLine
     */
    getLine: function(){
        return this._line ;
    },

    /**
     * Determines if you're at the end of the input.
     * @return {Boolean} True if there's no more input, false otherwise.
     * @method eof
     */
    eof: function(){
        return (this._cursor == this._input.length);
    },

    //-------------------------------------------------------------------------
    // Basic reading
    //-------------------------------------------------------------------------

    /**
     * Reads the next character without advancing the cursor.
     * @param {int} count How many characters to look ahead (default is 1).
     * @return {String} The next character or null if there is no next character.
     * @method peek
     */
    peek: function(count){
        var c = null;
        count = (typeof count == "undefined" ? 1 : count);

        //if we're not at the end of the input...
        if (this._cursor < this._input.length){

            //get character and increment cursor and column
            c = this._input.charAt(this._cursor + count - 1);
        }

        return c;
    },

    /**
     * Reads the next character from the input and adjusts the row and column
     * accordingly.
     * @return {String} The next character or null if there is no next character.
     * @method read
     */
    read: function(){
        var c = null;

        //if we're not at the end of the input...
        if (this._cursor < this._input.length){

            //if the last character was a newline, increment row count
            //and reset column count
            if (this._input.charAt(this._cursor) == "\n"){
                this._line++;
                this._col=1;
            } else {
                this._col++;
            }

            //get character and increment cursor and column
            c = this._input.charAt(this._cursor++);
        }

        return c;
    },

    //-------------------------------------------------------------------------
    // Misc
    //-------------------------------------------------------------------------

    /**
     * Saves the current location so it can be returned to later.
     * @method mark
     * @return {void}
     */
    mark: function(){
        this._bookmark = {
            cursor: this._cursor,
            line:   this._line,
            col:    this._col
        };
    },

    reset: function(){
        if (this._bookmark){
            this._cursor = this._bookmark.cursor;
            this._line = this._bookmark.line;
            this._col = this._bookmark.col;
            delete this._bookmark;
        }
    },

    //-------------------------------------------------------------------------
    // Advanced reading
    //-------------------------------------------------------------------------

    /**
     * Reads up to and including the given string. Throws an error if that
     * string is not found.
     * @param {String} pattern The string to read.
     * @return {String} The string when it is found.
     * @throws Error when the string pattern is not found.
     * @method readTo
     */
    readTo: function(pattern){

        var buffer = "",
            c;

        /*
         * First, buffer must be the same length as the pattern.
         * Then, buffer must end with the pattern or else reach the
         * end of the input.
         */
        while (buffer.length < pattern.length || buffer.lastIndexOf(pattern) != buffer.length - pattern.length){
            c = this.read();
            if (c){
                buffer += c;
            } else {
                throw new Error("Expected \"" + pattern + "\" at line " + this._line  + ", col " + this._col + ".");
            }
        }

        return buffer;

    },

    /**
     * Reads characters while each character causes the given
     * filter function to return true. The function is passed
     * in each character and either returns true to continue
     * reading or false to stop.
     * @param {Function} filter The function to read on each character.
     * @return {String} The string made up of all characters that passed the
     *      filter check.
     * @method readWhile
     */
    readWhile: function(filter){

        var buffer = "",
            c = this.read();

        while(c !== null && filter(c)){
            buffer += c;
            c = this.read();
        }

        return buffer;

    },

    /**
     * Reads characters that match either text or a regular expression and
     * returns those characters. If a match is found, the row and column
     * are adjusted; if no match is found, the reader's state is unchanged.
     * reading or false to stop.
     * @param {String|RegExp} matchter If a string, then the literal string
     *      value is searched for. If a regular expression, then any string
     *      matching the pattern is search for.
     * @return {String} The string made up of all characters that matched or
     *      null if there was no match.
     * @method readMatch
     */
    readMatch: function(matcher){

        var source = this._input.substring(this._cursor),
            value = null;

        //if it's a string, just do a straight match
        if (typeof matcher == "string"){
            if (source.indexOf(matcher) === 0){
                value = this.readCount(matcher.length);
            }
        } else if (matcher instanceof RegExp){
            if (matcher.test(source)){
                value = this.readCount(RegExp.lastMatch.length);
            }
        }

        return value;
    },


    /**
     * Reads a given number of characters. If the end of the input is reached,
     * it reads only the remaining characters and does not throw an error.
     * @param {int} count The number of characters to read.
     * @return {String} The string made up the read characters.
     * @method readCount
     */
    readCount: function(count){
        var buffer = "";

        while(count--){
            buffer += this.read();
        }

        return buffer;
    }

};
/**
 * Type to use when a syntax error occurs.
 * @class SyntaxError
 * @namespace parserlib.util
 * @constructor
 * @param {String} message The error message.
 * @param {int} line The line at which the error occurred.
 * @param {int} col The column at which the error occurred.
 */
function SyntaxError(message, line, col){

    /**
     * The column at which the error occurred.
     * @type int
     * @property col
     */
    this.col = col;

    /**
     * The line at which the error occurred.
     * @type int
     * @property line
     */
    this.line = line;

    /**
     * The text representation of the unit.
     * @type String
     * @property text
     */
    this.message = message;

}

//inherit from Error
SyntaxError.prototype = new Error();
/**
 * Base type to represent a single syntactic unit.
 * @class SyntaxUnit
 * @namespace parserlib.util
 * @constructor
 * @param {String} text The text of the unit.
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 */
function SyntaxUnit(text, line, col, type){


    /**
     * The column of text on which the unit resides.
     * @type int
     * @property col
     */
    this.col = col;

    /**
     * The line of text on which the unit resides.
     * @type int
     * @property line
     */
    this.line = line;

    /**
     * The text representation of the unit.
     * @type String
     * @property text
     */
    this.text = text;

    /**
     * The type of syntax unit.
     * @type int
     * @property type
     */
    this.type = type;
}

/**
 * Create a new syntax unit based solely on the given token.
 * Convenience method for creating a new syntax unit when
 * it represents a single token instead of multiple.
 * @param {Object} token The token object to represent.
 * @return {parserlib.util.SyntaxUnit} The object representing the token.
 * @static
 * @method fromToken
 */
SyntaxUnit.fromToken = function(token){
    return new SyntaxUnit(token.value, token.startLine, token.startCol);
};

SyntaxUnit.prototype = {

    //restore constructor
    constructor: SyntaxUnit,
    
    /**
     * Returns the text representation of the unit.
     * @return {String} The text representation of the unit.
     * @method valueOf
     */
    valueOf: function(){
        return this.toString();
    },
    
    /**
     * Returns the text representation of the unit.
     * @return {String} The text representation of the unit.
     * @method toString
     */
    toString: function(){
        return this.text;
    }

};
/**
 * Generic TokenStream providing base functionality.
 * @class TokenStreamBase
 * @namespace parserlib.util
 * @constructor
 * @param {String|StringReader} input The text to tokenize or a reader from 
 *      which to read the input.
 */
function TokenStreamBase(input, tokenData){

    /**
     * The string reader for easy access to the text.
     * @type StringReader
     * @property _reader
     * @private
     */
    //this._reader = (typeof input == "string") ? new StringReader(input) : input;
    this._reader = input ? new StringReader(input.toString()) : null;
    
    /**
     * Token object for the last consumed token.
     * @type Token
     * @property _token
     * @private
     */
    this._token = null;    
    
    /**
     * The array of token information.
     * @type Array
     * @property _tokenData
     * @private
     */
    this._tokenData = tokenData;
    
    /**
     * Lookahead token buffer.
     * @type Array
     * @property _lt
     * @private
     */
    this._lt = [];
    
    /**
     * Lookahead token buffer index.
     * @type int
     * @property _ltIndex
     * @private
     */
    this._ltIndex = 0;
    
    this._ltIndexCache = [];
}

/**
 * Accepts an array of token information and outputs
 * an array of token data containing key-value mappings
 * and matching functions that the TokenStream needs.
 * @param {Array} tokens An array of token descriptors.
 * @return {Array} An array of processed token data.
 * @method createTokenData
 * @static
 */
TokenStreamBase.createTokenData = function(tokens){

    var nameMap     = [],
        typeMap     = {},
        tokenData     = tokens.concat([]),
        i            = 0,
        len            = tokenData.length+1;
    
    tokenData.UNKNOWN = -1;
    tokenData.unshift({name:"EOF"});

    for (; i < len; i++){
        nameMap.push(tokenData[i].name);
        tokenData[tokenData[i].name] = i;
        if (tokenData[i].text){
            typeMap[tokenData[i].text] = i;
        }
    }
    
    tokenData.name = function(tt){
        return nameMap[tt];
    };
    
    tokenData.type = function(c){
        return typeMap[c];
    };
    
    return tokenData;
};

TokenStreamBase.prototype = {

    //restore constructor
    constructor: TokenStreamBase,    
    
    //-------------------------------------------------------------------------
    // Matching methods
    //-------------------------------------------------------------------------
    
    /**
     * Determines if the next token matches the given token type.
     * If so, that token is consumed; if not, the token is placed
     * back onto the token stream. You can pass in any number of
     * token types and this will return true if any of the token
     * types is found.
     * @param {int|int[]} tokenTypes Either a single token type or an array of
     *      token types that the next token might be. If an array is passed,
     *      it's assumed that the token can be any of these.
     * @param {variant} channel (Optional) The channel to read from. If not
     *      provided, reads from the default (unnamed) channel.
     * @return {Boolean} True if the token type matches, false if not.
     * @method match
     */
    match: function(tokenTypes, channel){
    
        //always convert to an array, makes things easier
        if (!(tokenTypes instanceof Array)){
            tokenTypes = [tokenTypes];
        }
                
        var tt  = this.get(channel),
            i   = 0,
            len = tokenTypes.length;
            
        while(i < len){
            if (tt == tokenTypes[i++]){
                return true;
            }
        }
        
        //no match found, put the token back
        this.unget();
        return false;
    },    
    
    /**
     * Determines if the next token matches the given token type.
     * If so, that token is consumed; if not, an error is thrown.
     * @param {int|int[]} tokenTypes Either a single token type or an array of
     *      token types that the next token should be. If an array is passed,
     *      it's assumed that the token must be one of these.
     * @param {variant} channel (Optional) The channel to read from. If not
     *      provided, reads from the default (unnamed) channel.
     * @return {void}
     * @method mustMatch
     */    
    mustMatch: function(tokenTypes, channel){

        var token;

        //always convert to an array, makes things easier
        if (!(tokenTypes instanceof Array)){
            tokenTypes = [tokenTypes];
        }

        if (!this.match.apply(this, arguments)){    
            token = this.LT(1);
            throw new SyntaxError("Expected " + this._tokenData[tokenTypes[0]].name + 
                " at line " + token.startLine + ", col " + token.startCol + ".", token.startLine, token.startCol);
        }
    },
    
    //-------------------------------------------------------------------------
    // Consuming methods
    //-------------------------------------------------------------------------
    
    /**
     * Keeps reading from the token stream until either one of the specified
     * token types is found or until the end of the input is reached.
     * @param {int|int[]} tokenTypes Either a single token type or an array of
     *      token types that the next token should be. If an array is passed,
     *      it's assumed that the token must be one of these.
     * @param {variant} channel (Optional) The channel to read from. If not
     *      provided, reads from the default (unnamed) channel.
     * @return {void}
     * @method advance
     */
    advance: function(tokenTypes, channel){
        
        while(this.LA(0) != 0 && !this.match(tokenTypes, channel)){
            this.get();
        }

        return this.LA(0);    
    },
    
    /**
     * Consumes the next token from the token stream. 
     * @return {int} The token type of the token that was just consumed.
     * @method get
     */      
    get: function(channel){
    
        var tokenInfo   = this._tokenData,
            reader      = this._reader,
            value,
            i           =0,
            len         = tokenInfo.length,
            found       = false,
            token,
            info;
            
        //check the lookahead buffer first
        if (this._lt.length && this._ltIndex >= 0 && this._ltIndex < this._lt.length){  
                           
            i++;
            this._token = this._lt[this._ltIndex++];
            info = tokenInfo[this._token.type];
            
            //obey channels logic
            while((info.channel !== undefined && channel !== info.channel) &&
                    this._ltIndex < this._lt.length){
                this._token = this._lt[this._ltIndex++];
                info = tokenInfo[this._token.type];
                i++;
            }
            
            //here be dragons
            if ((info.channel === undefined || channel === info.channel) &&
                    this._ltIndex <= this._lt.length){
                this._ltIndexCache.push(i);
                return this._token.type;
            }
        }
        
        //call token retriever method
        token = this._getToken();

        //if it should be hidden, don't save a token
        if (token.type > -1 && !tokenInfo[token.type].hide){
                     
            //apply token channel
            token.channel = tokenInfo[token.type].channel;
         
            //save for later
            this._token = token;
            this._lt.push(token);

            //save space that will be moved (must be done before array is truncated)
            this._ltIndexCache.push(this._lt.length - this._ltIndex + i);  
        
            //keep the buffer under 5 items
            if (this._lt.length > 5){
                this._lt.shift();                
            }
            
            //also keep the shift buffer under 5 items
            if (this._ltIndexCache.length > 5){
                this._ltIndexCache.shift();
            }
                
            //update lookahead index
            this._ltIndex = this._lt.length;
        }
            
        /*
         * Skip to the next token if:
         * 1. The token type is marked as hidden.
         * 2. The token type has a channel specified and it isn't the current channel.
         */
        info = tokenInfo[token.type];
        if (info && 
                (info.hide || 
                (info.channel !== undefined && channel !== info.channel))){
            return this.get(channel);
        } else {
            //return just the type
            return token.type;
        }
    },
    
    /**
     * Looks ahead a certain number of tokens and returns the token type at
     * that position. This will throw an error if you lookahead past the
     * end of input, past the size of the lookahead buffer, or back past
     * the first token in the lookahead buffer.
     * @param {int} The index of the token type to retrieve. 0 for the
     *      current token, 1 for the next, -1 for the previous, etc.
     * @return {int} The token type of the token in the given position.
     * @method LA
     */
    LA: function(index){
        var total = index,
            tt;
        if (index > 0){
            //TODO: Store 5 somewhere
            if (index > 5){
                throw new Error("Too much lookahead.");
            }
        
            //get all those tokens
            while(total){
                tt = this.get();   
                total--;                            
            }
            
            //unget all those tokens
            while(total < index){
                this.unget();
                total++;
            }
        } else if (index < 0){
        
            if(this._lt[this._ltIndex+index]){
                tt = this._lt[this._ltIndex+index].type;
            } else {
                throw new Error("Too much lookbehind.");
            }
        
        } else {
            tt = this._token.type;
        }
        
        return tt;
    
    },
    
    /**
     * Looks ahead a certain number of tokens and returns the token at
     * that position. This will throw an error if you lookahead past the
     * end of input, past the size of the lookahead buffer, or back past
     * the first token in the lookahead buffer.
     * @param {int} The index of the token type to retrieve. 0 for the
     *      current token, 1 for the next, -1 for the previous, etc.
     * @return {Object} The token of the token in the given position.
     * @method LA
     */    
    LT: function(index){
    
        //lookahead first to prime the token buffer
        this.LA(index);
        
        //now find the token, subtract one because _ltIndex is already at the next index
        return this._lt[this._ltIndex+index-1];    
    },
    
    /**
     * Returns the token type for the next token in the stream without 
     * consuming it.
     * @return {int} The token type of the next token in the stream.
     * @method peek
     */
    peek: function(){
        return this.LA(1);
    },
    
    /**
     * Returns the actual token object for the last consumed token.
     * @return {Token} The token object for the last consumed token.
     * @method token
     */
    token: function(){
        return this._token;
    },
    
    /**
     * Returns the name of the token for the given token type.
     * @param {int} tokenType The type of token to get the name of.
     * @return {String} The name of the token or "UNKNOWN_TOKEN" for any
     *      invalid token type.
     * @method tokenName
     */
    tokenName: function(tokenType){
        if (tokenType < 0 || tokenType > this._tokenData.length){
            return "UNKNOWN_TOKEN";
        } else {
            return this._tokenData[tokenType].name;
        }
    },
    
    /**
     * Returns the token type value for the given token name.
     * @param {String} tokenName The name of the token whose value should be returned.
     * @return {int} The token type value for the given token name or -1
     *      for an unknown token.
     * @method tokenName
     */    
    tokenType: function(tokenName){
        return this._tokenData[tokenName] || -1;
    },
    
    /**
     * Returns the last consumed token to the token stream.
     * @method unget
     */      
    unget: function(){
        //if (this._ltIndex > -1){
        if (this._ltIndexCache.length){
            this._ltIndex -= this._ltIndexCache.pop();//--;
            this._token = this._lt[this._ltIndex - 1];
        } else {
            throw new Error("Too much lookahead.");
        }
    }

};


parserlib.util = {
StringReader: StringReader,
SyntaxError : SyntaxError,
SyntaxUnit  : SyntaxUnit,
EventTarget : EventTarget,
TokenStreamBase : TokenStreamBase
};
})();
/* 
Parser-Lib
Copyright (c) 2009-2011 Nicholas C. Zakas. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
/* Build time: 21-November-2017 09:22:26 */
(function(){
var EventTarget = parserlib.util.EventTarget,
TokenStreamBase = parserlib.util.TokenStreamBase,
StringReader = parserlib.util.StringReader,
SyntaxError = parserlib.util.SyntaxError,
SyntaxUnit  = parserlib.util.SyntaxUnit;

var Colors = {
    aliceblue       :"#f0f8ff",
    antiquewhite    :"#faebd7",
    aqua            :"#00ffff",
    aquamarine      :"#7fffd4",
    azure           :"#f0ffff",
    beige           :"#f5f5dc",
    bisque          :"#ffe4c4",
    black           :"#000000",
    blanchedalmond  :"#ffebcd",
    blue            :"#0000ff",
    blueviolet      :"#8a2be2",
    brown           :"#a52a2a",
    burlywood       :"#deb887",
    cadetblue       :"#5f9ea0",
    chartreuse      :"#7fff00",
    chocolate       :"#d2691e",
    coral           :"#ff7f50",
    cornflowerblue  :"#6495ed",
    cornsilk        :"#fff8dc",
    crimson         :"#dc143c",
    cyan            :"#00ffff",
    darkblue        :"#00008b",
    darkcyan        :"#008b8b",
    darkgoldenrod   :"#b8860b",
    darkgray        :"#a9a9a9",
    darkgreen       :"#006400",
    darkkhaki       :"#bdb76b",
    darkmagenta     :"#8b008b",
    darkolivegreen  :"#556b2f",
    darkorange      :"#ff8c00",
    darkorchid      :"#9932cc",
    darkred         :"#8b0000",
    darksalmon      :"#e9967a",
    darkseagreen    :"#8fbc8f",
    darkslateblue   :"#483d8b",
    darkslategray   :"#2f4f4f",
    darkturquoise   :"#00ced1",
    darkviolet      :"#9400d3",
    deeppink        :"#ff1493",
    deepskyblue     :"#00bfff",
    dimgray         :"#696969",
    dodgerblue      :"#1e90ff",
    firebrick       :"#b22222",
    floralwhite     :"#fffaf0",
    forestgreen     :"#228b22",
    fuchsia         :"#ff00ff",
    gainsboro       :"#dcdcdc",
    ghostwhite      :"#f8f8ff",
    gold            :"#ffd700",
    goldenrod       :"#daa520",
    gray            :"#808080",
    green           :"#008000",
    greenyellow     :"#adff2f",
    honeydew        :"#f0fff0",
    hotpink         :"#ff69b4",
    indianred       :"#cd5c5c",
    indigo          :"#4b0082",
    ivory           :"#fffff0",
    khaki           :"#f0e68c",
    lavender        :"#e6e6fa",
    lavenderblush   :"#fff0f5",
    lawngreen       :"#7cfc00",
    lemonchiffon    :"#fffacd",
    lightblue       :"#add8e6",
    lightcoral      :"#f08080",
    lightcyan       :"#e0ffff",
    lightgoldenrodyellow  :"#fafad2",
    lightgrey       :"#d3d3d3",
    lightgreen      :"#90ee90",
    lightpink       :"#ffb6c1",
    lightsalmon     :"#ffa07a",
    lightseagreen   :"#20b2aa",
    lightskyblue    :"#87cefa",
    lightslategray  :"#778899",
    lightsteelblue  :"#b0c4de",
    lightyellow     :"#ffffe0",
    lime            :"#00ff00",
    limegreen       :"#32cd32",
    linen           :"#faf0e6",
    magenta         :"#ff00ff",
    maroon          :"#800000",
    mediumaquamarine:"#66cdaa",
    mediumblue      :"#0000cd",
    mediumorchid    :"#ba55d3",
    mediumpurple    :"#9370d8",
    mediumseagreen  :"#3cb371",
    mediumslateblue :"#7b68ee",
    mediumspringgreen   :"#00fa9a",
    mediumturquoise :"#48d1cc",
    mediumvioletred :"#c71585",
    midnightblue    :"#191970",
    mintcream       :"#f5fffa",
    mistyrose       :"#ffe4e1",
    moccasin        :"#ffe4b5",
    navajowhite     :"#ffdead",
    navy            :"#000080",
    oldlace         :"#fdf5e6",
    olive           :"#808000",
    olivedrab       :"#6b8e23",
    orange          :"#ffa500",
    orangered       :"#ff4500",
    orchid          :"#da70d6",
    palegoldenrod   :"#eee8aa",
    palegreen       :"#98fb98",
    paleturquoise   :"#afeeee",
    palevioletred   :"#d87093",
    papayawhip      :"#ffefd5",
    peachpuff       :"#ffdab9",
    peru            :"#cd853f",
    pink            :"#ffc0cb",
    plum            :"#dda0dd",
    powderblue      :"#b0e0e6",
    purple          :"#800080",
    red             :"#ff0000",
    rosybrown       :"#bc8f8f",
    royalblue       :"#4169e1",
    saddlebrown     :"#8b4513",
    salmon          :"#fa8072",
    sandybrown      :"#f4a460",
    seagreen        :"#2e8b57",
    seashell        :"#fff5ee",
    sienna          :"#a0522d",
    silver          :"#c0c0c0",
    skyblue         :"#87ceeb",
    slateblue       :"#6a5acd",
    slategray       :"#708090",
    snow            :"#fffafa",
    springgreen     :"#00ff7f",
    steelblue       :"#4682b4",
    tan             :"#d2b48c",
    teal            :"#008080",
    thistle         :"#d8bfd8",
    tomato          :"#ff6347",
    turquoise       :"#40e0d0",
    violet          :"#ee82ee",
    wheat           :"#f5deb3",
    white           :"#ffffff",
    whitesmoke      :"#f5f5f5",
    yellow          :"#ffff00",
    yellowgreen     :"#9acd32"
};
/**
 * Represents a selector combinator (whitespace, +, >).
 * @namespace parserlib.css
 * @class Combinator
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 * @param {String} text The text representation of the unit. 
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 */
function Combinator(text, line, col){
    
    SyntaxUnit.call(this, text, line, col, Parser.COMBINATOR_TYPE);

    /**
     * The type of modifier.
     * @type String
     * @property type
     */
    this.type = "unknown";
    
    //pretty simple
    if (/^\s+$/.test(text)){
        this.type = "descendant";
    } else if (text == ">"){
        this.type = "child";
    } else if (text == "+"){
        this.type = "adjacent-sibling";
    } else if (text == "~"){
        this.type = "sibling";
    }

}

Combinator.prototype = new SyntaxUnit();
Combinator.prototype.constructor = Combinator;

/**
 * Represents a media feature, such as max-width:500.
 * @namespace parserlib.css
 * @class MediaFeature
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 * @param {SyntaxUnit} name The name of the feature.
 * @param {SyntaxUnit} value The value of the feature or null if none.
 */
function MediaFeature(name, value){
    
    SyntaxUnit.call(this, "(" + name + (value !== null ? ":" + value : "") + ")", name.startLine, name.startCol, Parser.MEDIA_FEATURE_TYPE);

    /**
     * The name of the media feature
     * @type String
     * @property name
     */
    this.name = name;

    /**
     * The value for the feature or null if there is none.
     * @type SyntaxUnit
     * @property value
     */
    this.value = value;
}

MediaFeature.prototype = new SyntaxUnit();
MediaFeature.prototype.constructor = MediaFeature;

/**
 * Represents an individual media query.
 * @namespace parserlib.css
 * @class MediaQuery
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 * @param {String} modifier The modifier "not" or "only" (or null).
 * @param {String} mediaType The type of media (i.e., "print").
 * @param {Array} parts Array of selectors parts making up this selector.
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 */
function MediaQuery(modifier, mediaType, features, line, col){
    
    SyntaxUnit.call(this, (modifier ? modifier + " ": "") + (mediaType ? mediaType + " " : "") + features.join(" and "), line, col, Parser.MEDIA_QUERY_TYPE);

    /**
     * The media modifier ("not" or "only")
     * @type String
     * @property modifier
     */
    this.modifier = modifier;

    /**
     * The mediaType (i.e., "print")
     * @type String
     * @property mediaType
     */
    this.mediaType = mediaType;    
    
    /**
     * The parts that make up the selector.
     * @type Array
     * @property features
     */
    this.features = features;

}

MediaQuery.prototype = new SyntaxUnit();
MediaQuery.prototype.constructor = MediaQuery;

/**
 * A CSS3 parser.
 * @namespace parserlib.css
 * @class Parser
 * @constructor
 * @param {Object} options (Optional) Various options for the parser:
 *      starHack (true|false) to allow IE6 star hack as valid,
 *      underscoreHack (true|false) to interpret leading underscores
 *      as IE6-7 targeting for known properties, ieFilters (true|false)
 *      to indicate that IE < 8 filters should be accepted and not throw
 *      syntax errors.
 */
function Parser(options){

    //inherit event functionality
    EventTarget.call(this);


    this.options = options || {};

    this._tokenStream = null;
}

//Static constants
Parser.DEFAULT_TYPE = 0;
Parser.COMBINATOR_TYPE = 1;
Parser.MEDIA_FEATURE_TYPE = 2;
Parser.MEDIA_QUERY_TYPE = 3;
Parser.PROPERTY_NAME_TYPE = 4;
Parser.PROPERTY_VALUE_TYPE = 5;
Parser.PROPERTY_VALUE_PART_TYPE = 6;
Parser.SELECTOR_TYPE = 7;
Parser.SELECTOR_PART_TYPE = 8;
Parser.SELECTOR_SUB_PART_TYPE = 9;

Parser.prototype = function(){

    var proto = new EventTarget(),  //new prototype
        prop,
        additions =  {
        
            //restore constructor
            constructor: Parser,
                        
            //instance constants - yuck
            DEFAULT_TYPE : 0,
            COMBINATOR_TYPE : 1,
            MEDIA_FEATURE_TYPE : 2,
            MEDIA_QUERY_TYPE : 3,
            PROPERTY_NAME_TYPE : 4,
            PROPERTY_VALUE_TYPE : 5,
            PROPERTY_VALUE_PART_TYPE : 6,
            SELECTOR_TYPE : 7,
            SELECTOR_PART_TYPE : 8,
            SELECTOR_SUB_PART_TYPE : 9,            
        
            //-----------------------------------------------------------------
            // Grammar
            //-----------------------------------------------------------------
        
            _stylesheet: function(){
            
                /*
                 * stylesheet
                 *  : [ CHARSET_SYM S* STRING S* ';' ]?
                 *    [S|CDO|CDC]* [ import [S|CDO|CDC]* ]*
                 *    [ namespace [S|CDO|CDC]* ]*
                 *    [ [ ruleset | media | page | font_face | keyframes ] [S|CDO|CDC]* ]*
                 *  ;
                 */ 
               
                var tokenStream = this._tokenStream,
                    charset     = null,
                    token,
                    tt;
                    
                this.fire("startstylesheet");
            
                //try to read character set
                this._charset();
                
                this._skipCruft();

                //try to read imports - may be more than one
                while (tokenStream.peek() == Tokens.IMPORT_SYM){
                    this._import();
                    this._skipCruft();
                }
                
                //try to read namespaces - may be more than one
                while (tokenStream.peek() == Tokens.NAMESPACE_SYM){
                    this._namespace();
                    this._skipCruft();
                }
                
                //get the next token
                tt = tokenStream.peek();
                
                //try to read the rest
                while(tt > Tokens.EOF){
                
                    try {
                
                        switch(tt){
                            case Tokens.MEDIA_SYM:
                                this._media();
                                this._skipCruft();
                                break;
                            case Tokens.PAGE_SYM:
                                this._page(); 
                                this._skipCruft();
                                break;                   
                            case Tokens.FONT_FACE_SYM:
                                this._font_face(); 
                                this._skipCruft();
                                break;  
                            case Tokens.KEYFRAMES_SYM:
                                this._keyframes(); 
                                this._skipCruft();
                                break;  
                            case Tokens.S:
                                this._readWhitespace();
                                break;
                            default:                            
                                if(!this._ruleset()){
                                
                                    //error handling for known issues
                                    switch(tt){
                                        case Tokens.CHARSET_SYM:
                                            token = tokenStream.LT(1);
                                            this._charset(false);
                                            throw new SyntaxError("@charset not allowed here.", token.startLine, token.startCol);
                                        case Tokens.IMPORT_SYM:
                                            token = tokenStream.LT(1);
                                            this._import(false);
                                            throw new SyntaxError("@import not allowed here.", token.startLine, token.startCol);
                                        case Tokens.NAMESPACE_SYM:
                                            token = tokenStream.LT(1);
                                            this._namespace(false);
                                            throw new SyntaxError("@namespace not allowed here.", token.startLine, token.startCol);
                                        default:
                                            tokenStream.get();  //get the last token
                                            this._unexpectedToken(tokenStream.token());
                                    }
                                
                                }
                        }
                    } catch(ex) {
                        if (ex instanceof SyntaxError && !this.options.strict){
                            this.fire({
                                type:       "error",
                                error:      ex,
                                message:    ex.message,
                                line:       ex.line,
                                col:        ex.col
                            });                     
                        } else {
                            throw ex;
                        }
                    }
                    
                    tt = tokenStream.peek();
                }
                
                if (tt != Tokens.EOF){
                    this._unexpectedToken(tokenStream.token());
                }
            
                this.fire("endstylesheet");
            },
            
            _charset: function(emit){
                var tokenStream = this._tokenStream,
                    charset,
                    token,
                    line,
                    col;
                    
                if (tokenStream.match(Tokens.CHARSET_SYM)){
                    line = tokenStream.token().startLine;
                    col = tokenStream.token().startCol;
                
                    this._readWhitespace();
                    tokenStream.mustMatch(Tokens.STRING);
                    
                    token = tokenStream.token();
                    charset = token.value;
                    
                    this._readWhitespace();
                    tokenStream.mustMatch(Tokens.SEMICOLON);
                    
                    if (emit !== false){
                        this.fire({ 
                            type:   "charset",
                            charset:charset,
                            line:   line,
                            col:    col
                        });
                    }
                }            
            },
            
            _import: function(emit){
                /*
                 * import
                 *   : IMPORT_SYM S*
                 *    [STRING|URI] S* media_query_list? ';' S*
                 */    
            
                var tokenStream = this._tokenStream,
                    tt,
                    uri,
                    importToken,
                    mediaList   = [];
                
                //read import symbol
                tokenStream.mustMatch(Tokens.IMPORT_SYM);
                importToken = tokenStream.token();
                this._readWhitespace();
                
                tokenStream.mustMatch([Tokens.STRING, Tokens.URI]);
                
                //grab the URI value
                uri = tokenStream.token().value.replace(/(?:url\()?["']([^"']+)["']\)?/, "$1");                

                this._readWhitespace();
                
                mediaList = this._media_query_list();
                
                //must end with a semicolon
                tokenStream.mustMatch(Tokens.SEMICOLON);
                this._readWhitespace();
                
                if (emit !== false){
                    this.fire({
                        type:   "import",
                        uri:    uri,
                        media:  mediaList,
                        line:   importToken.startLine,
                        col:    importToken.startCol
                    });
                }
        
            },
            
            _namespace: function(emit){
                /*
                 * namespace
                 *   : NAMESPACE_SYM S* [namespace_prefix S*]? [STRING|URI] S* ';' S*
                 */    
            
                var tokenStream = this._tokenStream,
                    line,
                    col,
                    prefix,
                    uri;
                
                //read import symbol
                tokenStream.mustMatch(Tokens.NAMESPACE_SYM);
                line = tokenStream.token().startLine;
                col = tokenStream.token().startCol;
                this._readWhitespace();
                
                //it's a namespace prefix - no _namespace_prefix() method because it's just an IDENT
                if (tokenStream.match(Tokens.IDENT)){
                    prefix = tokenStream.token().value;
                    this._readWhitespace();
                }
                
                tokenStream.mustMatch([Tokens.STRING, Tokens.URI]);
                /*if (!tokenStream.match(Tokens.STRING)){
                    tokenStream.mustMatch(Tokens.URI);
                }*/
                
                //grab the URI value
                uri = tokenStream.token().value.replace(/(?:url\()?["']([^"']+)["']\)?/, "$1");                

                this._readWhitespace();

                //must end with a semicolon
                tokenStream.mustMatch(Tokens.SEMICOLON);
                this._readWhitespace();
                
                if (emit !== false){
                    this.fire({
                        type:   "namespace",
                        prefix: prefix,
                        uri:    uri,
                        line:   line,
                        col:    col
                    });
                }
        
            },            
                       
            _media: function(){
                /*
                 * media
                 *   : MEDIA_SYM S* media_query_list S* '{' S* ruleset* '}' S*
                 *   ;
                 */
                var tokenStream     = this._tokenStream,
                    line,
                    col,
                    mediaList;//       = [];
                
                //look for @media
                tokenStream.mustMatch(Tokens.MEDIA_SYM);
                line = tokenStream.token().startLine;
                col = tokenStream.token().startCol;
                
                this._readWhitespace();               

                mediaList = this._media_query_list();

                tokenStream.mustMatch(Tokens.LBRACE);
                this._readWhitespace();
                
                this.fire({
                    type:   "startmedia",
                    media:  mediaList,
                    line:   line,
                    col:    col
                });
                
                while(true) {
                    if (tokenStream.peek() == Tokens.PAGE_SYM){
                        this._page();
                    } else if (!this._ruleset()){
                        break;
                    }                
                }
                
                tokenStream.mustMatch(Tokens.RBRACE);
                this._readWhitespace();
        
                this.fire({
                    type:   "endmedia",
                    media:  mediaList,
                    line:   line,
                    col:    col
                });
            },                           
        

            //CSS3 Media Queries
            _media_query_list: function(){
                /*
                 * media_query_list
                 *   : S* [media_query [ ',' S* media_query ]* ]?
                 *   ;
                 */
                var tokenStream = this._tokenStream,
                    mediaList   = [];
                
                
                this._readWhitespace();
                
                if (tokenStream.peek() == Tokens.IDENT || tokenStream.peek() == Tokens.LPAREN){
                    mediaList.push(this._media_query());
                }
                
                while(tokenStream.match(Tokens.COMMA)){
                    this._readWhitespace();
                    mediaList.push(this._media_query());
                }
                
                return mediaList;
            },
            
            /*
             * Note: "expression" in the grammar maps to the _media_expression
             * method.
             
             */
            _media_query: function(){
                /*
                 * media_query
                 *   : [ONLY | NOT]? S* media_type S* [ AND S* expression ]*
                 *   | expression [ AND S* expression ]*
                 *   ;
                 */
                var tokenStream = this._tokenStream,
                    type        = null,
                    ident       = null,
                    token       = null,
                    expressions = [];
                    
                if (tokenStream.match(Tokens.IDENT)){
                    ident = tokenStream.token().value.toLowerCase();
                    
                    //since there's no custom tokens for these, need to manually check
                    if (ident != "only" && ident != "not"){
                        tokenStream.unget();
                        ident = null;
                    } else {
                        token = tokenStream.token();
                    }
                }
                                
                this._readWhitespace();
                
                if (tokenStream.peek() == Tokens.IDENT){
                    type = this._media_type();
                    if (token === null){
                        token = tokenStream.token();
                    }
                } else if (tokenStream.peek() == Tokens.LPAREN){
                    if (token === null){
                        token = tokenStream.LT(1);
                    }
                    expressions.push(this._media_expression());
                }                               
                
                if (type === null && expressions.length === 0){
                    return null;
                } else {                
                    this._readWhitespace();
                    while (tokenStream.match(Tokens.IDENT)){
                        if (tokenStream.token().value.toLowerCase() != "and"){
                            this._unexpectedToken(tokenStream.token());
                        }
                        
                        this._readWhitespace();
                        expressions.push(this._media_expression());
                    }
                }

                return new MediaQuery(ident, type, expressions, token.startLine, token.startCol);
            },

            //CSS3 Media Queries
            _media_type: function(){
                /*
                 * media_type
                 *   : IDENT
                 *   ;
                 */
                return this._media_feature();           
            },

            /**
             * Note: in CSS3 Media Queries, this is called "expression".
             * Renamed here to avoid conflict with CSS3 Selectors
             * definition of "expression". Also note that "expr" in the
             * grammar now maps to "expression" from CSS3 selectors.
             * @method _media_expression
             * @private
             */
            _media_expression: function(){
                /*
                 * expression
                 *  : '(' S* media_feature S* [ ':' S* expr ]? ')' S*
                 *  ;
                 */
                var tokenStream = this._tokenStream,
                    feature     = null,
                    token,
                    expression  = null;
                
                tokenStream.mustMatch(Tokens.LPAREN);
                
                feature = this._media_feature();
                this._readWhitespace();
                
                if (tokenStream.match(Tokens.COLON)){
                    this._readWhitespace();
                    token = tokenStream.LT(1);
                    expression = this._expression();
                }
                
                tokenStream.mustMatch(Tokens.RPAREN);
                this._readWhitespace();

                return new MediaFeature(feature, (expression ? new SyntaxUnit(expression, token.startLine, token.startCol) : null));            
            },

            //CSS3 Media Queries
            _media_feature: function(){
                /*
                 * media_feature
                 *   : IDENT
                 *   ;
                 */
                var tokenStream = this._tokenStream;
                    
                tokenStream.mustMatch(Tokens.IDENT);
                
                return SyntaxUnit.fromToken(tokenStream.token());            
            },
            
            //CSS3 Paged Media
            _page: function(){
                /*
                 * page:
                 *    PAGE_SYM S* IDENT? pseudo_page? S* 
                 *    '{' S* [ declaration | margin ]? [ ';' S* [ declaration | margin ]? ]* '}' S*
                 *    ;
                 */            
                var tokenStream = this._tokenStream,
                    line,
                    col,
                    identifier  = null,
                    pseudoPage  = null;
                
                //look for @page
                tokenStream.mustMatch(Tokens.PAGE_SYM);
                line = tokenStream.token().startLine;
                col = tokenStream.token().startCol;
                
                this._readWhitespace();
                
                if (tokenStream.match(Tokens.IDENT)){
                    identifier = tokenStream.token().value;

                    //The value 'auto' may not be used as a page name and MUST be treated as a syntax error.
                    if (identifier.toLowerCase() === "auto"){
                        this._unexpectedToken(tokenStream.token());
                    }
                }                
                
                //see if there's a colon upcoming
                if (tokenStream.peek() == Tokens.COLON){
                    pseudoPage = this._pseudo_page();
                }
            
                this._readWhitespace();
                
                this.fire({
                    type:   "startpage",
                    id:     identifier,
                    pseudo: pseudoPage,
                    line:   line,
                    col:    col
                });                   

                this._readDeclarations(true, true);                
                
                this.fire({
                    type:   "endpage",
                    id:     identifier,
                    pseudo: pseudoPage,
                    line:   line,
                    col:    col
                });             
            
            },
            
            //CSS3 Paged Media
            _margin: function(){
                /*
                 * margin :
                 *    margin_sym S* '{' declaration [ ';' S* declaration? ]* '}' S*
                 *    ;
                 */
                var tokenStream = this._tokenStream,
                    line,
                    col,
                    marginSym   = this._margin_sym();

                if (marginSym){
                    line = tokenStream.token().startLine;
                    col = tokenStream.token().startCol;
                
                    this.fire({
                        type: "startpagemargin",
                        margin: marginSym,
                        line:   line,
                        col:    col
                    });    
                    
                    this._readDeclarations(true);

                    this.fire({
                        type: "endpagemargin",
                        margin: marginSym,
                        line:   line,
                        col:    col
                    });    
                    return true;
                } else {
                    return false;
                }
            },

            //CSS3 Paged Media
            _margin_sym: function(){
            
                /*
                 * margin_sym :
                 *    TOPLEFTCORNER_SYM | 
                 *    TOPLEFT_SYM | 
                 *    TOPCENTER_SYM | 
                 *    TOPRIGHT_SYM | 
                 *    TOPRIGHTCORNER_SYM |
                 *    BOTTOMLEFTCORNER_SYM | 
                 *    BOTTOMLEFT_SYM | 
                 *    BOTTOMCENTER_SYM | 
                 *    BOTTOMRIGHT_SYM |
                 *    BOTTOMRIGHTCORNER_SYM |
                 *    LEFTTOP_SYM |
                 *    LEFTMIDDLE_SYM |
                 *    LEFTBOTTOM_SYM |
                 *    RIGHTTOP_SYM |
                 *    RIGHTMIDDLE_SYM |
                 *    RIGHTBOTTOM_SYM 
                 *    ;
                 */
            
                var tokenStream = this._tokenStream;
            
                if(tokenStream.match([Tokens.TOPLEFTCORNER_SYM, Tokens.TOPLEFT_SYM,
                        Tokens.TOPCENTER_SYM, Tokens.TOPRIGHT_SYM, Tokens.TOPRIGHTCORNER_SYM,
                        Tokens.BOTTOMLEFTCORNER_SYM, Tokens.BOTTOMLEFT_SYM, 
                        Tokens.BOTTOMCENTER_SYM, Tokens.BOTTOMRIGHT_SYM,
                        Tokens.BOTTOMRIGHTCORNER_SYM, Tokens.LEFTTOP_SYM, 
                        Tokens.LEFTMIDDLE_SYM, Tokens.LEFTBOTTOM_SYM, Tokens.RIGHTTOP_SYM,
                        Tokens.RIGHTMIDDLE_SYM, Tokens.RIGHTBOTTOM_SYM]))
                {
                    return SyntaxUnit.fromToken(tokenStream.token());                
                } else {
                    return null;
                }
            
            },
            
            _pseudo_page: function(){
                /*
                 * pseudo_page
                 *   : ':' IDENT
                 *   ;    
                 */
        
                var tokenStream = this._tokenStream;
                
                tokenStream.mustMatch(Tokens.COLON);
                tokenStream.mustMatch(Tokens.IDENT);
                
                //TODO: CSS3 Paged Media says only "left", "center", and "right" are allowed
                
                return tokenStream.token().value;
            },
            
            _font_face: function(){
                /*
                 * font_face
                 *   : FONT_FACE_SYM S* 
                 *     '{' S* declaration [ ';' S* declaration ]* '}' S*
                 *   ;
                 */     
                var tokenStream = this._tokenStream,
                    line,
                    col;
                
                //look for @page
                tokenStream.mustMatch(Tokens.FONT_FACE_SYM);
                line = tokenStream.token().startLine;
                col = tokenStream.token().startCol;
                
                this._readWhitespace();

                this.fire({
                    type:   "startfontface",
                    line:   line,
                    col:    col
                });                    
                
                this._readDeclarations(true);
                
                this.fire({
                    type:   "endfontface",
                    line:   line,
                    col:    col
                });              
            },

            _operator: function(){
            
                /*
                 * operator
                 *  : '/' S* | ',' S* | /( empty )/
                 *  ;
                 */    
                 
                var tokenStream = this._tokenStream,
                    token       = null;
                
                if (tokenStream.match([Tokens.SLASH, Tokens.COMMA])){
                    token =  tokenStream.token();
                    this._readWhitespace();
                } 
                return token ? PropertyValuePart.fromToken(token) : null;
                
            },
            
            _combinator: function(){
            
                /*
                 * combinator
                 *  : PLUS S* | GREATER S* | TILDE S* | S+
                 *  ;
                 */    
                 
                var tokenStream = this._tokenStream,
                    value       = null,
                    token;
                
                if(tokenStream.match([Tokens.PLUS, Tokens.GREATER, Tokens.TILDE])){                
                    token = tokenStream.token();
                    value = new Combinator(token.value, token.startLine, token.startCol);
                    this._readWhitespace();
                }
                
                return value;
            },
            
            _unary_operator: function(){
            
                /*
                 * unary_operator
                 *  : '-' | '+'
                 *  ;
                 */
                 
                var tokenStream = this._tokenStream;
                
                if (tokenStream.match([Tokens.MINUS, Tokens.PLUS])){
                    return tokenStream.token().value;
                } else {
                    return null;
                }         
            },
            
            _property: function(){
            
                /*
                 * property
                 *   : IDENT S*
                 *   ;        
                 */
                 
                var tokenStream = this._tokenStream,
                    value       = null,
                    hack        = null,
                    tokenValue,
                    token,
                    line,
                    col;
                    
                //check for star hack - throws error if not allowed
                if (tokenStream.peek() == Tokens.STAR && this.options.starHack){
                    tokenStream.get();
                    token = tokenStream.token();
                    hack = token.value;
                    line = token.startLine;
                    col = token.startCol;
                }
                
                if(tokenStream.match(Tokens.IDENT)){
                    token = tokenStream.token();
                    tokenValue = token.value;
                    
                    //check for underscore hack - no error if not allowed because it's valid CSS syntax
                    if (tokenValue.charAt(0) == "_" && this.options.underscoreHack){
                        hack = "_";
                        tokenValue = tokenValue.substring(1);
                    }
                    
                    value = new PropertyName(tokenValue, hack, (line||token.startLine), (col||token.startCol));
                    this._readWhitespace();
                }
                
                return value;
            },
        
            //Augmented with CSS3 Selectors
            _ruleset: function(){
                /*
                 * ruleset
                 *   : selectors_group
                 *     '{' S* declaration? [ ';' S* declaration? ]* '}' S*
                 *   ;    
                 */    
                 
                var tokenStream = this._tokenStream,
                    tt,
                    selectors;


                /*
                 * Error Recovery: If even a single selector fails to parse,
                 * then the entire ruleset should be thrown away.
                 */
                try {
                    selectors = this._selectors_group();
                } catch (ex){
                    if (ex instanceof SyntaxError && !this.options.strict){
                    
                        //fire error event
                        this.fire({
                            type:       "error",
                            error:      ex,
                            message:    ex.message,
                            line:       ex.line,
                            col:        ex.col
                        });                          
                        
                        //skip over everything until closing brace
                        tt = tokenStream.advance([Tokens.RBRACE]);
                        if (tt == Tokens.RBRACE){
                            //if there's a right brace, the rule is finished so don't do anything
                        } else {
                            //otherwise, rethrow the error because it wasn't handled properly
                            throw ex;
                        }                        
                        
                    } else {
                        //not a syntax error, rethrow it
                        throw ex;
                    }                
                
                    //trigger parser to continue
                    return true;
                }
                
                //if it got here, all selectors parsed
                if (selectors){ 
                                    
                    this.fire({
                        type:       "startrule",
                        selectors:  selectors,
                        line:       selectors[0].line,
                        col:        selectors[0].col
                    });                
                    
                    this._readDeclarations(true);                
                    
                    this.fire({
                        type:       "endrule",
                        selectors:  selectors,
                        line:       selectors[0].line,
                        col:        selectors[0].col
                    });  
                    
                }
                
                return selectors;
                
            },

            //CSS3 Selectors
            _selectors_group: function(){
            
                /*            
                 * selectors_group
                 *   : selector [ COMMA S* selector ]*
                 *   ;
                 */           
                var tokenStream = this._tokenStream,
                    selectors   = [],
                    selector;
                    
                selector = this._selector();
                if (selector !== null){
                
                    selectors.push(selector);
                    while(tokenStream.match(Tokens.COMMA)){
                        this._readWhitespace();
                        selector = this._selector();
                        if (selector !== null){
                            selectors.push(selector);
                        } else {
                            this._unexpectedToken(tokenStream.LT(1));
                        }
                    }
                }

                return selectors.length ? selectors : null;
            },
                
            //CSS3 Selectors
            _selector: function(){
                /*
                 * selector
                 *   : simple_selector_sequence [ combinator simple_selector_sequence ]*
                 *   ;    
                 */
                 
                var tokenStream = this._tokenStream,
                    selector    = [],
                    nextSelector = null,
                    combinator  = null,
                    ws          = null;
                
                //if there's no simple selector, then there's no selector
                nextSelector = this._simple_selector_sequence();
                if (nextSelector === null){
                    return null;
                }
                
                selector.push(nextSelector);
                
                do {
                    
                    //look for a combinator
                    combinator = this._combinator();
                    
                    if (combinator !== null){
                        selector.push(combinator);
                        nextSelector = this._simple_selector_sequence();
                        
                        //there must be a next selector
                        if (nextSelector === null){
                            this._unexpectedToken(this.LT(1));
                        } else {
                        
                            //nextSelector is an instance of SelectorPart
                            selector.push(nextSelector);
                        }
                    } else {
                        
                        //if there's not whitespace, we're done
                        if (this._readWhitespace()){           
        
                            //add whitespace separator
                            ws = new Combinator(tokenStream.token().value, tokenStream.token().startLine, tokenStream.token().startCol);
                            
                            //combinator is not required
                            combinator = this._combinator();
                            
                            //selector is required if there's a combinator
                            nextSelector = this._simple_selector_sequence();
                            if (nextSelector === null){                        
                                if (combinator !== null){
                                    this._unexpectedToken(tokenStream.LT(1));
                                }
                            } else {
                                
                                if (combinator !== null){
                                    selector.push(combinator);
                                } else {
                                    selector.push(ws);
                                }
                                
                                selector.push(nextSelector);
                            }     
                        } else {
                            break;
                        }               
                    
                    }
                } while(true);
                
                return new Selector(selector, selector[0].line, selector[0].col);
            },
            
            //CSS3 Selectors
            _simple_selector_sequence: function(){
                /*
                 * simple_selector_sequence
                 *   : [ type_selector | universal ]
                 *     [ HASH | class | attrib | pseudo | negation ]*
                 *   | [ HASH | class | attrib | pseudo | negation ]+
                 *   ;
                 */
                 
                var tokenStream = this._tokenStream,
                
                    //parts of a simple selector
                    elementName = null,
                    modifiers   = [],
                    
                    //complete selector text
                    selectorText= "",

                    //the different parts after the element name to search for
                    components  = [
                        //HASH
                        function(){
                            return tokenStream.match(Tokens.HASH) ?
                                    new SelectorSubPart(tokenStream.token().value, "id", tokenStream.token().startLine, tokenStream.token().startCol) :
                                    null;
                        },
                        this._class,
                        this._attrib,
                        this._pseudo,
                        this._negation
                    ],
                    i           = 0,
                    len         = components.length,
                    component   = null,
                    found       = false,
                    line,
                    col;
                    
                    
                //get starting line and column for the selector
                line = tokenStream.LT(1).startLine;
                col = tokenStream.LT(1).startCol;
                                        
                elementName = this._type_selector();
                if (!elementName){
                    elementName = this._universal();
                }
                
                if (elementName !== null){
                    selectorText += elementName;
                }                
                
                while(true){

                    //whitespace means we're done
                    if (tokenStream.peek() === Tokens.S){
                        break;
                    }
                
                    //check for each component
                    while(i < len && component === null){
                        component = components[i++].call(this);
                    }
        
                    if (component === null){
                    
                        //we don't have a selector
                        if (selectorText === ""){
                            return null;
                        } else {
                            break;
                        }
                    } else {
                        i = 0;
                        modifiers.push(component);
                        selectorText += component.toString(); 
                        component = null;
                    }
                }

                 
                return selectorText !== "" ?
                        new SelectorPart(elementName, modifiers, selectorText, line, col) :
                        null;
            },            
            
            //CSS3 Selectors
            _type_selector: function(){
                /*
                 * type_selector
                 *   : [ namespace_prefix ]? element_name
                 *   ;
                 */
                 
                var tokenStream = this._tokenStream,
                    ns          = this._namespace_prefix(),
                    elementName = this._element_name();
                    
                if (!elementName){                    
                    /*
                     * Need to back out the namespace that was read due to both
                     * type_selector and universal reading namespace_prefix
                     * first. Kind of hacky, but only way I can figure out
                     * right now how to not change the grammar.
                     */
                    if (ns){
                        tokenStream.unget();
                        if (ns.length > 1){
                            tokenStream.unget();
                        }
                    }
                
                    return null;
                } else {     
                    if (ns){
                        elementName.text = ns + elementName.text;
                        elementName.col -= ns.length;
                    }
                    return elementName;
                }
            },
            
            //CSS3 Selectors
            _class: function(){
                /*
                 * class
                 *   : '.' IDENT
                 *   ;
                 */    
                 
                var tokenStream = this._tokenStream,
                    token;
                
                if (tokenStream.match(Tokens.DOT)){
                    tokenStream.mustMatch(Tokens.IDENT);    
                    token = tokenStream.token();
                    return new SelectorSubPart("." + token.value, "class", token.startLine, token.startCol - 1);        
                } else {
                    return null;
                }
        
            },
            
            //CSS3 Selectors
            _element_name: function(){
                /*
                 * element_name
                 *   : IDENT
                 *   ;
                 */    
                
                var tokenStream = this._tokenStream,
                    token;
                
                if (tokenStream.match(Tokens.IDENT)){
                    token = tokenStream.token();
                    return new SelectorSubPart(token.value, "elementName", token.startLine, token.startCol);        
                
                } else {
                    return null;
                }
            },
            
            //CSS3 Selectors
            _namespace_prefix: function(){
                /*            
                 * namespace_prefix
                 *   : [ IDENT | '*' ]? '|'
                 *   ;
                 */
                var tokenStream = this._tokenStream,
                    value       = "";
                    
                //verify that this is a namespace prefix
                if (tokenStream.LA(1) === Tokens.PIPE || tokenStream.LA(2) === Tokens.PIPE){
                        
                    if(tokenStream.match([Tokens.IDENT, Tokens.STAR])){
                        value += tokenStream.token().value;
                    }
                    
                    tokenStream.mustMatch(Tokens.PIPE);
                    value += "|";
                    
                }
                
                return value.length ? value : null;                
            },
            
            //CSS3 Selectors
            _universal: function(){
                /*
                 * universal
                 *   : [ namespace_prefix ]? '*'
                 *   ;            
                 */
                var tokenStream = this._tokenStream,
                    value       = "",
                    ns;
                    
                ns = this._namespace_prefix();
                if(ns){
                    value += ns;
                }
                
                if(tokenStream.match(Tokens.STAR)){
                    value += "*";
                }
                
                return value.length ? value : null;
                
           },
            
            //CSS3 Selectors
            _attrib: function(){
                /*
                 * attrib
                 *   : '[' S* [ namespace_prefix ]? IDENT S*
                 *         [ [ PREFIXMATCH |
                 *             SUFFIXMATCH |
                 *             SUBSTRINGMATCH |
                 *             '=' |
                 *             INCLUDES |
                 *             DASHMATCH ] S* [ IDENT | STRING ] S*
                 *         ]? ']'
                 *   ;    
                 */
                 
                var tokenStream = this._tokenStream,
                    value       = null,
                    ns,
                    token;
                
                if (tokenStream.match(Tokens.LBRACKET)){
                    token = tokenStream.token();
                    value = token.value;
                    value += this._readWhitespace();
                    
                    ns = this._namespace_prefix();
                    
                    if (ns){
                        value += ns;
                    }
                                        
                    tokenStream.mustMatch(Tokens.IDENT);
                    value += tokenStream.token().value;                    
                    value += this._readWhitespace();
                    
                    if(tokenStream.match([Tokens.PREFIXMATCH, Tokens.SUFFIXMATCH, Tokens.SUBSTRINGMATCH,
                            Tokens.EQUALS, Tokens.INCLUDES, Tokens.DASHMATCH])){
                    
                        value += tokenStream.token().value;                    
                        value += this._readWhitespace();
                        
                        tokenStream.mustMatch([Tokens.IDENT, Tokens.STRING]);
                        value += tokenStream.token().value;                    
                        value += this._readWhitespace();
                    }
                    
                    tokenStream.mustMatch(Tokens.RBRACKET);
                                        
                    return new SelectorSubPart(value + "]", "attribute", token.startLine, token.startCol);
                } else {
                    return null;
                }
            },
            
            //CSS3 Selectors
            _pseudo: function(){
            
                /*
                 * pseudo
                 *   : ':' ':'? [ IDENT | functional_pseudo ]
                 *   ;    
                 */   
            
                var tokenStream = this._tokenStream,
                    pseudo      = null,
                    colons      = ":",
                    line,
                    col;
                
                if (tokenStream.match(Tokens.COLON)){
                
                    if (tokenStream.match(Tokens.COLON)){
                        colons += ":";
                    }
                
                    if (tokenStream.match(Tokens.IDENT)){
                        pseudo = tokenStream.token().value;
                        line = tokenStream.token().startLine;
                        col = tokenStream.token().startCol - colons.length;
                    } else if (tokenStream.peek() == Tokens.FUNCTION){
                        line = tokenStream.LT(1).startLine;
                        col = tokenStream.LT(1).startCol - colons.length;
                        pseudo = this._functional_pseudo();
                    }
                    
                    if (pseudo){
                        pseudo = new SelectorSubPart(colons + pseudo, "pseudo", line, col);
                    }
                }
        
                return pseudo;
            },
            
            //CSS3 Selectors
            _functional_pseudo: function(){
                /*
                 * functional_pseudo
                 *   : FUNCTION S* expression ')'
                 *   ;
                */            
                
                var tokenStream = this._tokenStream,
                    value = null;
                
                if(tokenStream.match(Tokens.FUNCTION)){
                    value = tokenStream.token().value;
                    value += this._readWhitespace();
                    value += this._expression();
                    tokenStream.mustMatch(Tokens.RPAREN);
                    value += ")";
                }
                
                return value;
            },
            
            //CSS3 Selectors
            _expression: function(){
                /*
                 * expression
                 *   : [ [ PLUS | '-' | DIMENSION | NUMBER | STRING | IDENT ] S* ]+
                 *   ;
                 */
                 
                var tokenStream = this._tokenStream,
                    value       = "";
                    
                while(tokenStream.match([Tokens.PLUS, Tokens.MINUS, Tokens.DIMENSION,
                        Tokens.NUMBER, Tokens.STRING, Tokens.IDENT, Tokens.LENGTH,
                        Tokens.FREQ, Tokens.ANGLE, Tokens.TIME,
                        Tokens.RESOLUTION])){
                    
                    value += tokenStream.token().value;
                    value += this._readWhitespace();                        
                }
                
                return value.length ? value : null;
                
            },

            //CSS3 Selectors
            _negation: function(){
                /*            
                 * negation
                 *   : NOT S* negation_arg S* ')'
                 *   ;
                 */

                var tokenStream = this._tokenStream,
                    line,
                    col,
                    value       = "",
                    arg,
                    subpart     = null;
                    
                if (tokenStream.match(Tokens.NOT)){
                    value = tokenStream.token().value;
                    line = tokenStream.token().startLine;
                    col = tokenStream.token().startCol;
                    value += this._readWhitespace();
                    arg = this._negation_arg();
                    value += arg;
                    value += this._readWhitespace();
                    tokenStream.match(Tokens.RPAREN);
                    value += tokenStream.token().value;
                    
                    subpart = new SelectorSubPart(value, "not", line, col);
                    subpart.args.push(arg);
                }
                
                return subpart;
            },
            
            //CSS3 Selectors
            _negation_arg: function(){            
                /*
                 * negation_arg
                 *   : type_selector | universal | HASH | class | attrib | pseudo
                 *   ;            
                 */           
                 
                var tokenStream = this._tokenStream,
                    args        = [
                        this._type_selector,
                        this._universal,
                        function(){
                            return tokenStream.match(Tokens.HASH) ?
                                    new SelectorSubPart(tokenStream.token().value, "id", tokenStream.token().startLine, tokenStream.token().startCol) :
                                    null;                        
                        },
                        this._class,
                        this._attrib,
                        this._pseudo                    
                    ],
                    arg         = null,
                    i           = 0,
                    len         = args.length,
                    elementName,
                    line,
                    col,
                    part;
                    
                line = tokenStream.LT(1).startLine;
                col = tokenStream.LT(1).startCol;
                
                while(i < len && arg === null){
                    
                    arg = args[i].call(this);
                    i++;
                }
                
                //must be a negation arg
                if (arg === null){
                    this._unexpectedToken(tokenStream.LT(1));
                }
 
                //it's an element name
                if (arg.type == "elementName"){
                    part = new SelectorPart(arg, [], arg.toString(), line, col);
                } else {
                    part = new SelectorPart(null, [arg], arg.toString(), line, col);
                }
                
                return part;                
            },
            
            _declaration: function(){
            
                /*
                 * declaration
                 *   : property ':' S* expr prio?
                 *   | /( empty )/
                 *   ;     
                 */    
            
                var tokenStream = this._tokenStream,
                    property    = null,
                    expr        = null,
                    prio        = null,
                    error       = null,
                    invalid     = null;
                
                property = this._property();
                if (property !== null){

                    tokenStream.mustMatch(Tokens.COLON);
                    this._readWhitespace();
                    
                    expr = this._expr();
                    
                    //if there's no parts for the value, it's an error
                    if (!expr || expr.length === 0){
                        this._unexpectedToken(tokenStream.LT(1));
                    }
                    
                    prio = this._prio();
                    
                    try {
                        this._validateProperty(property, expr);
                    } catch (ex) {
                        invalid = ex;
                    }
                    
                    this.fire({
                        type:       "property",
                        property:   property,
                        value:      expr,
                        important:  prio,
                        line:       property.line,
                        col:        property.col,
                        invalid:    invalid
                    });                      
                    
                    return true;
                } else {
                    return false;
                }
            },
            
            _prio: function(){
                /*
                 * prio
                 *   : IMPORTANT_SYM S*
                 *   ;    
                 */
                 
                var tokenStream = this._tokenStream,
                    result      = tokenStream.match(Tokens.IMPORTANT_SYM);
                    
                this._readWhitespace();
                return result;
            },
            
            _expr: function(){
                /*
                 * expr
                 *   : term [ operator term ]*
                 *   ;
                 */
        
                var tokenStream = this._tokenStream,
                    values      = [],
					//valueParts	= [],
                    value       = null,
                    operator    = null;
                    
                value = this._term();
                if (value !== null){
                
                    values.push(value);
                    
                    do {
                        operator = this._operator();
        
                        //if there's an operator, keep building up the value parts
                        if (operator){
                            values.push(operator);
                        } /*else {
                            //if there's not an operator, you have a full value
							values.push(new PropertyValue(valueParts, valueParts[0].line, valueParts[0].col));
							valueParts = [];
						}*/
                        
                        value = this._term();
                        
                        if (value === null){
                            break;
                        } else {
                            values.push(value);
                        }
                    } while(true);
                }
				
				//cleanup
                /*if (valueParts.length){
                    values.push(new PropertyValue(valueParts, valueParts[0].line, valueParts[0].col));
                }*/
        
                return values.length > 0 ? new PropertyValue(values, values[0].startLine, values[0].startCol) : null;
            },
            
            _term: function(){                       
            
                /*
                 * term
                 *   : unary_operator?
                 *     [ NUMBER S* | PERCENTAGE S* | LENGTH S* | ANGLE S* |
                 *       TIME S* | FREQ S* | function | ie_function ]
                 *   | STRING S* | IDENT S* | URI S* | UNICODERANGE S* | hexcolor
                 *   ;
                 */    
        
                var tokenStream = this._tokenStream,
                    unary       = null,
                    value       = null,
                    line,
                    col;
                    
                //returns the operator or null
                unary = this._unary_operator();
                if (unary !== null){
                    line = tokenStream.token().startLine;
                    col = tokenStream.token().startCol;
                }                
               
                //exception for IE filters
                if (tokenStream.peek() == Tokens.IE_FUNCTION && this.options.ieFilters){
                
                    value = this._ie_function();
                    if (unary === null){
                        line = tokenStream.token().startLine;
                        col = tokenStream.token().startCol;
                    }
                
                //see if there's a simple match
                } else if (tokenStream.match([Tokens.NUMBER, Tokens.PERCENTAGE, Tokens.LENGTH,
                        Tokens.ANGLE, Tokens.TIME,
                        Tokens.FREQ, Tokens.STRING, Tokens.IDENT, Tokens.URI, Tokens.UNICODE_RANGE])){
                 
                    value = tokenStream.token().value;
                    if (unary === null){
                        line = tokenStream.token().startLine;
                        col = tokenStream.token().startCol;
                    }
                    this._readWhitespace();
                } else {
                
                    //see if it's a color
                    value = this._hexcolor();
                    if (value === null){
                    
                        //if there's no unary, get the start of the next token for line/col info
                        if (unary === null){
                            line = tokenStream.LT(1).startLine;
                            col = tokenStream.LT(1).startCol;
                        }                    
                    
                        //has to be a function
                        if (value === null){
                            
                            /*
                             * This checks for alpha(opacity=0) style of IE
                             * functions. IE_FUNCTION only presents progid: style.
                             */
                            if (tokenStream.LA(3) == Tokens.EQUALS && this.options.ieFilters){
                                value = this._ie_function();
                            } else {
                                value = this._function();
                            }
                        }

                        /*if (value === null){
                            return null;
                            //throw new Error("Expected identifier at line " + tokenStream.token().startLine + ", character " +  tokenStream.token().startCol + ".");
                        }*/
                    
                    } else {
                        if (unary === null){
                            line = tokenStream.token().startLine;
                            col = tokenStream.token().startCol;
                        }                    
                    }
                
                }                
                
                return value !== null ?
                        new PropertyValuePart(unary !== null ? unary + value : value, line, col) :
                        null;
        
            },
            
            _function: function(){
            
                /*
                 * function
                 *   : FUNCTION S* expr ')' S*
                 *   ;
                 */
                 
                var tokenStream = this._tokenStream,
                    functionText = null,
                    expr        = null,
                    lt;
                    
                if (tokenStream.match(Tokens.FUNCTION)){
                    functionText = tokenStream.token().value;
                    this._readWhitespace();
                    expr = this._expr();
                    functionText += expr;
                    
                    //START: Horrible hack in case it's an IE filter
                    if (this.options.ieFilters && tokenStream.peek() == Tokens.EQUALS){
                        do {
                        
                            if (this._readWhitespace()){
                                functionText += tokenStream.token().value;
                            }
                            
                            //might be second time in the loop
                            if (tokenStream.LA(0) == Tokens.COMMA){
                                functionText += tokenStream.token().value;
                            }
                        
                            tokenStream.match(Tokens.IDENT);
                            functionText += tokenStream.token().value;
                            
                            tokenStream.match(Tokens.EQUALS);
                            functionText += tokenStream.token().value;
                            
                            //functionText += this._term();
                            lt = tokenStream.peek();
                            while(lt != Tokens.COMMA && lt != Tokens.S && lt != Tokens.RPAREN){
                                tokenStream.get();
                                functionText += tokenStream.token().value;
                                lt = tokenStream.peek();
                            }
                        } while(tokenStream.match([Tokens.COMMA, Tokens.S]));
                    }

                    //END: Horrible Hack
                    
                    tokenStream.match(Tokens.RPAREN);    
                    functionText += ")";
                    this._readWhitespace();
                }                
                
                return functionText;
            }, 
            
            _ie_function: function(){
            
                /* (My own extension)
                 * ie_function
                 *   : IE_FUNCTION S* IDENT '=' term [S* ','? IDENT '=' term]+ ')' S*
                 *   ;
                 */
                 
                var tokenStream = this._tokenStream,
                    functionText = null,
                    expr        = null,
                    lt;
                    
                //IE function can begin like a regular function, too
                if (tokenStream.match([Tokens.IE_FUNCTION, Tokens.FUNCTION])){
                    functionText = tokenStream.token().value;
                    
                    do {
                    
                        if (this._readWhitespace()){
                            functionText += tokenStream.token().value;
                        }
                        
                        //might be second time in the loop
                        if (tokenStream.LA(0) == Tokens.COMMA){
                            functionText += tokenStream.token().value;
                        }
                    
                        tokenStream.match(Tokens.IDENT);
                        functionText += tokenStream.token().value;
                        
                        tokenStream.match(Tokens.EQUALS);
                        functionText += tokenStream.token().value;
                        
                        //functionText += this._term();
                        lt = tokenStream.peek();
                        while(lt != Tokens.COMMA && lt != Tokens.S && lt != Tokens.RPAREN){
                            tokenStream.get();
                            functionText += tokenStream.token().value;
                            lt = tokenStream.peek();
                        }
                    } while(tokenStream.match([Tokens.COMMA, Tokens.S]));                    
                    
                    tokenStream.match(Tokens.RPAREN);    
                    functionText += ")";
                    this._readWhitespace();
                }                
                
                return functionText;
            }, 
            
            _hexcolor: function(){
                /*
                 * There is a constraint on the color that it must
                 * have either 3 or 6 hex-digits (i.e., [0-9a-fA-F])
                 * after the "#"; e.g., "#000" is OK, but "#abcd" is not.
                 *
                 * hexcolor
                 *   : HASH S*
                 *   ;
                 */
                 
                var tokenStream = this._tokenStream,
                    token,
                    color = null;
                
                if(tokenStream.match(Tokens.HASH)){
                
                    //need to do some validation here
                    
                    token = tokenStream.token();
                    color = token.value;
                    if (!/#[a-f0-9]{3,6}/i.test(color)){
                        throw new SyntaxError("Expected a hex color but found '" + color + "' at line " + token.startLine + ", col " + token.startCol + ".", token.startLine, token.startCol);
                    }
                    this._readWhitespace();
                }
                
                return color;
            },
            
            //-----------------------------------------------------------------
            // Animations methods
            //-----------------------------------------------------------------
            
            _keyframes: function(){
            
                /*
                 * keyframes:
                 *   : KEYFRAMES_SYM S* keyframe_name S* '{' S* keyframe_rule* '}' {
                 *   ;
                 */
                var tokenStream = this._tokenStream,
                    token,
                    tt,
                    name;            
                    
                tokenStream.mustMatch(Tokens.KEYFRAMES_SYM);
                this._readWhitespace();
                name = this._keyframe_name();
                
                this._readWhitespace();
                tokenStream.mustMatch(Tokens.LBRACE);
                    
                this.fire({
                    type:   "startkeyframes",
                    name:   name,
                    line:   name.line,
                    col:    name.col
                });                
                
                this._readWhitespace();
                tt = tokenStream.peek();
                
                //check for key
                while(tt == Tokens.IDENT || tt == Tokens.PERCENTAGE) {
                    this._keyframe_rule();
                    this._readWhitespace();
                    tt = tokenStream.peek();
                }           
                
                this.fire({
                    type:   "endkeyframes",
                    name:   name,
                    line:   name.line,
                    col:    name.col
                });                      
                    
                this._readWhitespace();
                tokenStream.mustMatch(Tokens.RBRACE);                    
                
            },
            
            _keyframe_name: function(){
            
                /*
                 * keyframe_name:
                 *   : IDENT
                 *   | STRING
                 *   ;
                 */
                var tokenStream = this._tokenStream,
                    token;

                tokenStream.mustMatch([Tokens.IDENT, Tokens.STRING]);
                return SyntaxUnit.fromToken(tokenStream.token());            
            },
            
            _keyframe_rule: function(){
            
                /*
                 * keyframe_rule:
                 *   : key_list S* 
                 *     '{' S* declaration [ ';' S* declaration ]* '}' S*
                 *   ;
                 */
                var tokenStream = this._tokenStream,
                    token,
                    keyList = this._key_list();
                                    
                this.fire({
                    type:   "startkeyframerule",
                    keys:   keyList,
                    line:   keyList[0].line,
                    col:    keyList[0].col
                });                
                
                this._readDeclarations(true);                
                
                this.fire({
                    type:   "endkeyframerule",
                    keys:   keyList,
                    line:   keyList[0].line,
                    col:    keyList[0].col
                });  
                
            },
            
            _key_list: function(){
            
                /*
                 * key_list:
                 *   : key [ S* ',' S* key]*
                 *   ;
                 */
                var tokenStream = this._tokenStream,
                    token,
                    key,
                    keyList = [];
                    
                //must be least one key
                keyList.push(this._key());
                    
                this._readWhitespace();
                    
                while(tokenStream.match(Tokens.COMMA)){
                    this._readWhitespace();
                    keyList.push(this._key());
                    this._readWhitespace();
                }

                return keyList;
            },
                        
            _key: function(){
                /*
                 * There is a restriction that IDENT can be only "from" or "to".
                 *
                 * key
                 *   : PERCENTAGE
                 *   | IDENT
                 *   ;
                 */
                 
                var tokenStream = this._tokenStream,
                    token;
                    
                if (tokenStream.match(Tokens.PERCENTAGE)){
                    return SyntaxUnit.fromToken(tokenStream.token());
                } else if (tokenStream.match(Tokens.IDENT)){
                    token = tokenStream.token();                    
                    
                    if (/from|to/i.test(token.value)){
                        return SyntaxUnit.fromToken(token);
                    }
                    
                    tokenStream.unget();
                }
                
                //if it gets here, there wasn't a valid token, so time to explode
                this._unexpectedToken(tokenStream.LT(1));
            },
            
            //-----------------------------------------------------------------
            // Helper methods
            //-----------------------------------------------------------------
            
            /**
             * Not part of CSS grammar, but useful for skipping over
             * combination of white space and HTML-style comments.
             * @return {void}
             * @method _skipCruft
             * @private
             */
            _skipCruft: function(){
                while(this._tokenStream.match([Tokens.S, Tokens.CDO, Tokens.CDC])){
                    //noop
                }
            },

            /**
             * Not part of CSS grammar, but this pattern occurs frequently
             * in the official CSS grammar. Split out here to eliminate
             * duplicate code.
             * @param {Boolean} checkStart Indicates if the rule should check
             *      for the left brace at the beginning.
             * @param {Boolean} readMargins Indicates if the rule should check
             *      for margin patterns.
             * @return {void}
             * @method _readDeclarations
             * @private
             */
            _readDeclarations: function(checkStart, readMargins){
                /*
                 * Reads the pattern
                 * S* '{' S* declaration [ ';' S* declaration ]* '}' S*
                 * or
                 * S* '{' S* [ declaration | margin ]? [ ';' S* [ declaration | margin ]? ]* '}' S*
                 * Note that this is how it is described in CSS3 Paged Media, but is actually incorrect.
                 * A semicolon is only necessary following a delcaration is there's another declaration
                 * or margin afterwards. 
                 */
                var tokenStream = this._tokenStream,
                    tt;
                       

                this._readWhitespace();
                
                if (checkStart){
                    tokenStream.mustMatch(Tokens.LBRACE);            
                }
                
                this._readWhitespace();

                try {
                    
                    while(true){
                    
                        if (readMargins && this._margin()){
                            //noop
                        } else if (this._declaration()){
                            if (!tokenStream.match(Tokens.SEMICOLON)){
                                break;
                            }
                        } else {
                            break;
                        }
                    
                        //if ((!this._margin() && !this._declaration()) || !tokenStream.match(Tokens.SEMICOLON)){
                        //    break;
                        //}
                        this._readWhitespace();
                    }
                    
                    tokenStream.mustMatch(Tokens.RBRACE);
                    this._readWhitespace();
                    
                } catch (ex) {
                    if (ex instanceof SyntaxError && !this.options.strict){
                    
                        //fire error event
                        this.fire({
                            type:       "error",
                            error:      ex,
                            message:    ex.message,
                            line:       ex.line,
                            col:        ex.col
                        });                          
                        
                        //see if there's another declaration
                        tt = tokenStream.advance([Tokens.SEMICOLON, Tokens.RBRACE]);
                        if (tt == Tokens.SEMICOLON){
                            //if there's a semicolon, then there might be another declaration
                            this._readDeclarations(false, readMargins);
                        } else if (tt == Tokens.RBRACE){
                            //if there's a right brace, the rule is finished so don't do anything
                        } else {
                            //otherwise, rethrow the error because it wasn't handled properly
                            throw ex;
                        }                        
                        
                    } else {
                        //not a syntax error, rethrow it
                        throw ex;
                    }
                }    
            
            },      
            
            /**
             * In some cases, you can end up with two white space tokens in a
             * row. Instead of making a change in every function that looks for
             * white space, this function is used to match as much white space
             * as necessary.
             * @method _readWhitespace
             * @return {String} The white space if found, empty string if not.
             * @private
             */
            _readWhitespace: function(){
            
                var tokenStream = this._tokenStream,
                    ws = "";
                    
                while(tokenStream.match(Tokens.S)){
                    ws += tokenStream.token().value;
                }
                
                return ws;
            },
          

            /**
             * Throws an error when an unexpected token is found.
             * @param {Object} token The token that was found.
             * @method _unexpectedToken
             * @return {void}
             * @private
             */
            _unexpectedToken: function(token){
                throw new SyntaxError("Unexpected token '" + token.value + "' at line " + token.startLine + ", col " + token.startCol + ".", token.startLine, token.startCol);
            },
            
            /**
             * Helper method used for parsing subparts of a style sheet.
             * @return {void}
             * @method _verifyEnd
             * @private
             */
            _verifyEnd: function(){
                if (this._tokenStream.LA(1) != Tokens.EOF){
                    this._unexpectedToken(this._tokenStream.LT(1));
                }            
            },
            
            //-----------------------------------------------------------------
            // Validation methods
            //-----------------------------------------------------------------
            _validateProperty: function(property, value){
                var name = property.text.toLowerCase(),
                    validation,
                    i, len;
                
                if (Properties[name]){
                
                    if (typeof Properties[name] == "function"){
                        Properties[name](value);                   
                    } 
                    
                    //otherwise, no validation available yet
                } else if (name.indexOf("-") !== 0){    //vendor prefixed are ok
                    throw new ValidationError("Unknown property '" + property + "'.", property.line, property.col);
                }
            },
            
            //-----------------------------------------------------------------
            // Parsing methods
            //-----------------------------------------------------------------
            
            parse: function(input){    
                this._tokenStream = new TokenStream(input, Tokens);
                this._stylesheet();
            },
            
            parseStyleSheet: function(input){
                //just passthrough
                return this.parse(input);
            },
            
            parseMediaQuery: function(input){
                this._tokenStream = new TokenStream(input, Tokens);
                var result = this._media_query();
                
                //if there's anything more, then it's an invalid selector
                this._verifyEnd();
                
                //otherwise return result
                return result;            
            },
            
            /**
             * Parses a property value (everything after the semicolon).
             * @return {parserlib.css.PropertyValue} The property value.
             * @throws parserlib.util.SyntaxError If an unexpected token is found.
             * @method parserPropertyValue
             */             
            parsePropertyValue: function(input){
            
                this._tokenStream = new TokenStream(input, Tokens);
                this._readWhitespace();
                
                var result = this._expr();
                
                //okay to have a trailing white space
                this._readWhitespace();
                
                //if there's anything more, then it's an invalid selector
                this._verifyEnd();
                
                //otherwise return result
                return result;
            },
            
            /**
             * Parses a complete CSS rule, including selectors and
             * properties.
             * @param {String} input The text to parser.
             * @return {Boolean} True if the parse completed successfully, false if not.
             * @method parseRule
             */
            parseRule: function(input){
                this._tokenStream = new TokenStream(input, Tokens);
                
                //skip any leading white space
                this._readWhitespace();
                
                var result = this._ruleset();
                
                //skip any trailing white space
                this._readWhitespace();

                //if there's anything more, then it's an invalid selector
                this._verifyEnd();
                
                //otherwise return result
                return result;            
            },
            
            /**
             * Parses a single CSS selector (no comma)
             * @param {String} input The text to parse as a CSS selector.
             * @return {Selector} An object representing the selector.
             * @throws parserlib.util.SyntaxError If an unexpected token is found.
             * @method parseSelector
             */
            parseSelector: function(input){
            
                this._tokenStream = new TokenStream(input, Tokens);
                
                //skip any leading white space
                this._readWhitespace();
                
                var result = this._selector();
                
                //skip any trailing white space
                this._readWhitespace();

                //if there's anything more, then it's an invalid selector
                this._verifyEnd();
                
                //otherwise return result
                return result;
            },

            /**
             * Parses an HTML style attribute: a set of CSS declarations 
             * separated by semicolons.
             * @param {String} input The text to parse as a style attribute
             * @return {void} 
             * @method parseStyleAttribute
             */
            parseStyleAttribute: function(input){
                input += "}"; // for error recovery in _readDeclarations()
                this._tokenStream = new TokenStream(input, Tokens);
                this._readDeclarations();
            }
        };
        
    //copy over onto prototype
    for (prop in additions){
        proto[prop] = additions[prop];
    }   
    
    return proto;
}();


/*
nth
  : S* [ ['-'|'+']? INTEGER? {N} [ S* ['-'|'+'] S* INTEGER ]? |
         ['-'|'+']? INTEGER | {O}{D}{D} | {E}{V}{E}{N} ] S*
  ;
*/
//This file will likely change a lot! Very experimental!

var ValidationType = {

    "absolute-size": function(part){
        return this.identifier(part, "xx-small | x-small | small | medium | large | x-large | xx-large");
    },
    
    "attachment": function(part){
        return this.identifier(part, "scroll | fixed | local");
    },
    
    "box": function(part){
        return this.identifier(part, "padding-box | border-box | content-box");
    },
    
    "relative-size": function(part){
        return this.identifier(part, "smaller | larger");
    },
    
    "identifier": function(part, options){
        var text = part.text.toString().toLowerCase(),
            args = options.split(" | "),
            i, len, found = false;

        
        for (i=0,len=args.length; i < len && !found; i++){
            if (text == args[i]){
                found = true;
            }
        }
        
        return found;
    },
    
    "length": function(part){
        return part.type == "length" || part.type == "number" || part.type == "integer" || part == "0";
    },
    
    "color": function(part){
        return part.type == "color" || part == "transparent";
    },
    
    "number": function(part){
        return part.type == "number" || this.integer(part);
    },
    
    "integer": function(part){
        return part.type == "integer";
    },
    
    "angle": function(part){
        return part.type == "angle";
    },        
    
    "uri": function(part){
        return part.type == "uri";
    },
    
    "image": function(part){
        return this.uri(part);
    },
    
    "bg-image": function(part){
        return this.image(part) || part == "none";
    },
    
    "percentage": function(part){
        return part.type == "percentage" || part == "0";
    },

    "border-width": function(part){
        return this.length(part) || this.identifier(part, "thin | medium | thick");
    },
    
    "border-style": function(part){
        return this.identifier(part, "none | hidden | dotted | dashed | solid | double | groove | ridge | inset | outset");
    },
    
    "margin-width": function(part){
        return this.length(part) || this.percentage(part) || this.identifier(part, "auto");
    },
    
    "padding-width": function(part){
        return this.length(part) || this.percentage(part);
    }
};

    
       




var Properties = {

    //A
    "alignment-adjust": 1,
    "alignment-baseline": 1,
    "animation": 1,
    "animation-delay": 1,
    "animation-direction":          { multi: [ "normal | alternate" ], separator: "," },
    "animation-duration": 1,
    "animation-fill-mode": 1,
    "animation-iteration-count":    { multi: [ "number", "infinite"], separator: "," },
    "animation-name": 1,
    "animation-play-state":         { multi: [ "running | paused" ], separator: "," },
    "animation-timing-function": 1,
    "appearance": 1,
    "azimuth": 1,
    
    //B
    "backface-visibility": 1,
    "background": 1,
    "background-attachment":        { multi: [ "attachment" ], separator: "," },
    "background-break": 1,
    "background-clip":              { multi: [ "box" ], separator: "," },
    "background-color":             [ "color", "inherit" ],
    "background-image":             { multi: [ "bg-image" ], separator: "," },
    "background-origin":            { multi: [ "box" ], separator: "," },
    "background-position": 1,
    "background-repeat":            [ "repeat | repeat-x | repeat-y | no-repeat | inherit" ],
    "background-size": 1,
    "baseline-shift": 1,
    "binding": 1,
    "bleed": 1,
    "bookmark-label": 1,
    "bookmark-level": 1,
    "bookmark-state": 1,
    "bookmark-target": 1,
    "border": 1,
    "border-bottom": 1,
    "border-bottom-color": 1,
    "border-bottom-left-radius":    1,
    "border-bottom-right-radius":   1,
    "border-bottom-style":          [ "border-style" ],
    "border-bottom-width":          [ "border-width" ],
    "border-collapse":              [ "collapse | separate | inherit" ],
    "border-color":                 { multi: [ "color", "inherit" ], max: 4 },
    "border-image": 1,
    "border-image-outset":          { multi: [ "length", "number" ], max: 4 },
    "border-image-repeat":          { multi: [ "stretch | repeat | round" ], max: 2 },
    "border-image-slice": 1,
    "border-image-source":          [ "image", "none" ],
    "border-image-width":           { multi: [ "length", "percentage", "number", "auto" ], max: 4 },
    "border-left": 1,
    "border-left-color":            [ "color", "inherit" ],
    "border-left-style":            [ "border-style" ],
    "border-left-width":            [ "border-width" ],
    "border-radius": 1,
    "border-right": 1,
    "border-right-color":           [ "color", "inherit" ],
    "border-right-style":           [ "border-style" ],
    "border-right-width":           [ "border-width" ],
    "border-spacing": 1,
    "border-style":                 { multi: [ "border-style" ], max: 4 },
    "border-top": 1,
    "border-top-color":             [ "color", "inherit" ],
    "border-top-left-radius": 1,
    "border-top-right-radius": 1,
    "border-top-style":             [ "border-style" ],
    "border-top-width":             [ "border-width" ],
    "border-width":                 { multi: [ "border-width" ], max: 4 },
    "bottom":                       [ "margin-width", "inherit" ], 
    "box-align":                    [ "start | end | center | baseline | stretch" ],        //http://www.w3.org/TR/2009/WD-css3-flexbox-20090723/
    "box-decoration-break":         [ "slice |clone" ],
    "box-direction":                [ "normal | reverse | inherit" ],
    "box-flex":                     [ "number" ],
    "box-flex-group":               [ "integer" ],
    "box-lines":                    [ "single | multiple" ],
    "box-ordinal-group":            [ "integer" ],
    "box-orient":                   [ "horizontal | vertical | inline-axis | block-axis | inherit" ],
    "box-pack":                     [ "start | end | center | justify" ],
    "box-shadow": 1,
    "box-sizing":                   [ "content-box | border-box | inherit" ],
    "break-after":                  [ "auto | always | avoid | left | right | page | column | avoid-page | avoid-column" ],
    "break-before":                 [ "auto | always | avoid | left | right | page | column | avoid-page | avoid-column" ],
    "break-inside":                 [ "auto | avoid | avoid-page | avoid-column" ],
    
    //C
    "caption-side":                 [ "top | bottom | inherit" ],
    "clear":                        [ "none | right | left | both | inherit" ],
    "clip": 1,
    "color":                        [ "color", "inherit" ],
    "color-profile": 1,
    "column-count":                 [ "integer", "auto" ],                      //http://www.w3.org/TR/css3-multicol/
    "column-fill":                  [ "auto | balance" ],
    "column-gap":                   [ "length", "normal" ],
    "column-rule": 1,
    "column-rule-color":            [ "color" ],
    "column-rule-style":            [ "border-style" ],
    "column-rule-width":            [ "border-width" ],
    "column-span":                  [ "none | all" ],
    "column-width":                 [ "length", "auto" ],
    "columns": 1,
    "content": 1,
    "counter-increment": 1,
    "counter-reset": 1,
    "crop": 1,
    "cue":                          [ "cue-after | cue-before | inherit" ],
    "cue-after": 1,
    "cue-before": 1,
    "cursor": 1,
    
    //D
    "direction":                    [ "ltr | rtl | inherit" ],
    "display":                      [ "inline | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | box | inline-box | grid | inline-grid", "none | inherit" ],
    "dominant-baseline": 1,
    "drop-initial-after-adjust": 1,
    "drop-initial-after-align": 1,
    "drop-initial-before-adjust": 1,
    "drop-initial-before-align": 1,
    "drop-initial-size": 1,
    "drop-initial-value": 1,
    
    //E
    "elevation": 1,
    "empty-cells":                  [ "show | hide | inherit" ],
    
    //F
    "filter": 1,
    "fit":                          [ "fill | hidden | meet | slice" ],
    "fit-position": 1,
    "float":                        [ "left | right | none | inherit" ],    
    "float-offset": 1,
    "font": 1,
    "font-family": 1,
    "font-size":                    [ "absolute-size", "relative-size", "length", "percentage", "inherit" ],
    "font-size-adjust": 1,
    "font-stretch": 1,
    "font-style":                   [ "normal | italic | oblique | inherit" ],
    "font-variant":                 [ "normal | small-caps | inherit" ],
    "font-weight":                  [ "normal | bold | bolder | lighter | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900 | inherit" ],
    
    //G
    "grid-cell-stacking":           [ "columns | rows | layer" ],
    "grid-column": 1,
    "grid-columns": 1,
    "grid-column-align":            [ "start | end | center | stretch" ],
    "grid-column-sizing": 1,
    "grid-column-span":             [ "integer" ],
    "grid-flow":                    [ "none | rows | columns" ],
    "grid-layer":                   [ "integer" ],
    "grid-row": 1,
    "grid-rows": 1,
    "grid-row-align":               [ "start | end | center | stretch" ],
    "grid-row-span":                [ "integer" ],
    "grid-row-sizing": 1,
    
    //H
    "hanging-punctuation": 1,
    "height":                       [ "margin-width", "inherit" ],
    "hyphenate-after": 1,
    "hyphenate-before": 1,
    "hyphenate-character":          [ "string", "auto" ],
    "hyphenate-lines": 1,
    "hyphenate-resource": 1,
    "hyphens":                      [ "none | manual | auto" ],
    
    //I
    "icon": 1,
    "image-orientation":            [ "angle", "auto" ],
    "image-rendering": 1,
    "image-resolution": 1,
    "inline-box-align": 1,
    
    //L
    "left":                         [ "margin-width", "inherit" ],
    "letter-spacing":               [ "length", "normal | inherit" ],
    "line-height":                  [ "number", "length", "percentage", "normal | inherit"],
    "line-break":                   [ "auto | loose | normal | strict" ],
    "line-stacking": 1,
    "line-stacking-ruby": 1,
    "line-stacking-shift": 1,
    "line-stacking-strategy": 1,
    "list-style": 1,
    "list-style-image":             [ "uri", "none | inherit" ],
    "list-style-position":          [ "inside | outside | inherit" ],
    "list-style-type":              [ "disc | circle | square | decimal | decimal-leading-zero | lower-roman | upper-roman | lower-greek | lower-latin | upper-latin | armenian | georgian | lower-alpha | upper-alpha | none | inherit" ],
    
    //M
    "margin":                       { multi: [ "margin-width", "inherit" ], max: 4 },
    "margin-bottom":                [ "margin-width", "inherit" ],
    "margin-left":                  [ "margin-width", "inherit" ],
    "margin-right":                 [ "margin-width", "inherit" ],
    "margin-top":                   [ "margin-width", "inherit" ],
    "mark": 1,
    "mark-after": 1,
    "mark-before": 1,
    "marks": 1,
    "marquee-direction": 1,
    "marquee-play-count": 1,
    "marquee-speed": 1,
    "marquee-style": 1,
    "max-height":                   [ "length", "percentage", "none | inherit" ],
    "max-width":                    [ "length", "percentage", "none | inherit" ],
    "min-height":                   [ "length", "percentage", "inherit" ],
    "min-width":                    [ "length", "percentage", "inherit" ],
    "move-to": 1,
    
    //N
    "nav-down": 1,
    "nav-index": 1,
    "nav-left": 1,
    "nav-right": 1,
    "nav-up": 1,
    
    //O
    "opacity":                      [ "number", "inherit" ],
    "orphans":                      [ "integer", "inherit" ],
    "outline": 1,
    "outline-color":                [ "color", "invert | inherit" ],
    "outline-offset": 1,
    "outline-style":                [ "border-style", "inherit" ],
    "outline-width":                [ "border-width", "inherit" ],
    "overflow":                     [ "visible | hidden | scroll | auto | inherit" ],
    "overflow-style": 1,
    "overflow-x": 1,
    "overflow-y": 1,
    
    //P
    "padding":                      { multi: [ "padding-width", "inherit" ], max: 4 },
    "padding-bottom":               [ "padding-width", "inherit" ],
    "padding-left":                 [ "padding-width", "inherit" ],
    "padding-right":                [ "padding-width", "inherit" ],
    "padding-top":                  [ "padding-width", "inherit" ],
    "page": 1,
    "page-break-after":             [ "auto | always | avoid | left | right | inherit" ],
    "page-break-before":            [ "auto | always | avoid | left | right | inherit" ],
    "page-break-inside":            [ "auto | avoid | inherit" ],
    "page-policy": 1,
    "pause": 1,
    "pause-after": 1,
    "pause-before": 1,
    "perspective": 1,
    "perspective-origin": 1,
    "phonemes": 1,
    "pitch": 1,
    "pitch-range": 1,
    "play-during": 1,
    "position":                     [ "static | relative | absolute | fixed | inherit" ],
    "presentation-level": 1,
    "punctuation-trim": 1,
    
    //Q
    "quotes": 1,
    
    //R
    "rendering-intent": 1,
    "resize": 1,
    "rest": 1,
    "rest-after": 1,
    "rest-before": 1,
    "richness": 1,
    "right":                        [ "margin-width", "inherit" ],
    "rotation": 1,
    "rotation-point": 1,
    "ruby-align": 1,
    "ruby-overhang": 1,
    "ruby-position": 1,
    "ruby-span": 1,
    
    //S
    "size": 1,
    "speak":                        [ "normal | none | spell-out | inherit" ],
    "speak-header":                 [ "once | always | inherit" ],
    "speak-numeral":                [ "digits | continuous | inherit" ],
    "speak-punctuation":            [ "code | none | inherit" ],
    "speech-rate": 1,
    "src" : 1,
    "stress": 1,
    "string-set": 1,
    
    "table-layout":                 [ "auto | fixed | inherit" ],
    "tab-size":                     [ "integer", "length" ],
    "target": 1,
    "target-name": 1,
    "target-new": 1,
    "target-position": 1,
    "text-align":                   [ "left | right | center | justify | inherit" ],
    "text-align-last": 1,
    "text-decoration": 1,
    "text-emphasis": 1,
    "text-height": 1,
    "text-indent":                  [ "length", "percentage", "inherit" ],
    "text-justify":                 [ "auto | none | inter-word | inter-ideograph | inter-cluster | distribute | kashida" ],
    "text-outline": 1,
    "text-overflow": 1,
    "text-shadow": 1,
    "text-transform":               [ "capitalize | uppercase | lowercase | none | inherit" ],
    "text-wrap":                    [ "normal | none | avoid" ],
    "top":                          [ "margin-width", "inherit" ],
    "transform": 1,
    "transform-origin": 1,
    "transform-style": 1,
    "transition": 1,
    "transition-delay": 1,
    "transition-duration": 1,
    "transition-property": 1,
    "transition-timing-function": 1,
    
    //U
    "unicode-bidi":                 [ "normal | embed | bidi-override | inherit" ],
    "user-modify":                  [ "read-only | read-write | write-only | inherit" ],
    "user-select":                  [ "none | text | toggle | element | elements | all | inherit" ],
    
    //V
    "vertical-align":               [ "percentage", "length", "baseline | sub | super | top | text-top | middle | bottom | text-bottom | inherit" ],
    "visibility":                   [ "visible | hidden | collapse | inherit" ],
    "voice-balance": 1,
    "voice-duration": 1,
    "voice-family": 1,
    "voice-pitch": 1,
    "voice-pitch-range": 1,
    "voice-rate": 1,
    "voice-stress": 1,
    "voice-volume": 1,
    "volume": 1,
    
    //W
    "white-space":                  [ "normal | pre | nowrap | pre-wrap | pre-line | inherit" ],
    "white-space-collapse": 1,
    "widows":                       [ "integer", "inherit" ],
    "width":                        [ "length", "percentage", "auto", "inherit" ],
    "word-break":                   [ "normal | keep-all | break-all" ],
    "word-spacing":                 [ "length", "normal | inherit" ],
    "word-wrap": 1,
    
    //Z
    "z-index":                      [ "integer", "auto | inherit" ],
    "zoom":                         [ "number", "percentage", "normal" ]
};

//Create validation functions for strings
(function(){
    var prop;
    for (prop in Properties){
        if (Properties.hasOwnProperty(prop)){
            if (Properties[prop] instanceof Array){
                Properties[prop] = (function(values){
                    return function(value){
                        var valid   = false,
                            msg     = [],
                            part    = value.parts[0];
                        
                        if (value.parts.length != 1){
                            throw new ValidationError("Expected 1 value but found " + value.parts.length + ".", value.line, value.col);
                        }
                        
                        for (var i=0, len=values.length; i < len && !valid; i++){
                            if (typeof ValidationType[values[i]] == "undefined"){
                                valid = valid || ValidationType.identifier(part, values[i]);
                                msg.push("one of (" + values[i] + ")");
                            } else {
                                valid = valid || ValidationType[values[i]](part);
                                msg.push(values[i]);
                            }
                        }
                        
                        if (!valid){
                            throw new ValidationError("Expected " + msg.join(" or ") + " but found '" + value + "'.", value.line, value.col);
                        }
                    };
                })(Properties[prop]);
            } else if (typeof Properties[prop] == "object"){
                Properties[prop] = (function(spec){
                    return function(value){
                        var valid,
                            i, len, j, count,
                            msg,
                            values,
                            last,
                            parts   = value.parts;
                        
                        //if there's a maximum set, use it (max can't be 0)
                        if (spec.max) {
                            if (parts.length > spec.max){
                                throw new ValidationError("Expected a max of " + spec.max + " property values but found " + parts.length + ".", value.line, value.col);
                            }
                        }
                        
                        if (spec.multi){
                            values = spec.multi;                            
                        }
                        
                        for (i=0, len=parts.length; i < len; i++){
                            msg = [];
                            valid = false;
                            
                            if (spec.separator && parts[i].type == "operator"){
                                
                                //two operators in a row - not allowed?
                                if ((last && last.type == "operator")){
                                    msg = msg.concat(values);
                                } else if (i == len-1){
                                    msg = msg.concat("end of line");
                                } else if (parts[i] != spec.separator){
                                    msg.push("'" + spec.separator + "'");
                                } else {
                                    valid = true;
                                }
                            } else {

                                for (j=0, count=values.length; j < count; j++){
                                    if (typeof ValidationType[values[j]] == "undefined"){
                                        if(ValidationType.identifier(parts[i], values[j])){
                                            valid = true;
                                            break;
                                        }
                                        msg.push("one of (" + values[j] + ")");
                                    } else {
                                        if (ValidationType[values[j]](parts[i])){
                                            valid = true;
                                            break;
                                        }
                                        msg.push(values[j]);
                                    }                                   
                                }
                            }

                            
                            if (!valid) {
                                throw new ValidationError("Expected " + msg.join(" or ") + " but found '" + parts[i] + "'.", value.line, value.col);
                            }
                            
                            
                            last = parts[i];
                        }                

                    };
                })(Properties[prop]);                
            }
        }
    }
})();
/**
 * Represents a selector combinator (whitespace, +, >).
 * @namespace parserlib.css
 * @class PropertyName
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 * @param {String} text The text representation of the unit. 
 * @param {String} hack The type of IE hack applied ("*", "_", or null).
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 */
function PropertyName(text, hack, line, col){
    
    SyntaxUnit.call(this, text, line, col, Parser.PROPERTY_NAME_TYPE);

    /**
     * The type of IE hack applied ("*", "_", or null).
     * @type String
     * @property hack
     */
    this.hack = hack;

}

PropertyName.prototype = new SyntaxUnit();
PropertyName.prototype.constructor = PropertyName;
PropertyName.prototype.toString = function(){
    return (this.hack ? this.hack : "") + this.text;
};
/**
 * Represents a single part of a CSS property value, meaning that it represents
 * just everything single part between ":" and ";". If there are multiple values
 * separated by commas, this type represents just one of the values.
 * @param {String[]} parts An array of value parts making up this value.
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 * @namespace parserlib.css
 * @class PropertyValue
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 */
function PropertyValue(parts, line, col){

    SyntaxUnit.call(this, parts.join(" "), line, col, Parser.PROPERTY_VALUE_TYPE);
    
    /**
     * The parts that make up the selector.
     * @type Array
     * @property parts
     */
    this.parts = parts;
    
}

PropertyValue.prototype = new SyntaxUnit();
PropertyValue.prototype.constructor = PropertyValue;

/**
 * Represents a single part of a CSS property value, meaning that it represents
 * just one part of the data between ":" and ";".
 * @param {String} text The text representation of the unit.
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 * @namespace parserlib.css
 * @class PropertyValuePart
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 */
function PropertyValuePart(text, line, col){

    SyntaxUnit.call(this, text, line, col, Parser.PROPERTY_VALUE_PART_TYPE);
    
    /**
     * Indicates the type of value unit.
     * @type String
     * @property type
     */
    this.type = "unknown";

    //figure out what type of data it is
    
    var temp;
    
    //it is a measurement?
    if (/^([+\-]?[\d\.]+)([a-z]+)$/i.test(text)){  //dimension
        this.type = "dimension";
        this.value = +RegExp.$1;
        this.units = RegExp.$2;
        
        //try to narrow down
        switch(this.units.toLowerCase()){
        
            case "em":
            case "rem":
            case "ex":
            case "px":
            case "cm":
            case "mm":
            case "in":
            case "pt":
            case "pc":
                this.type = "length";
                break;
                
            case "deg":
            case "rad":
            case "grad":
                this.type = "angle";
                break;
            
            case "ms":
            case "s":
                this.type = "time";
                break;
            
            case "hz":
            case "khz":
                this.type = "frequency";
                break;
            
            case "dpi":
            case "dpcm":
                this.type = "resolution";
                break;
                
            //default
                
        }
        
    } else if (/^([+\-]?[\d\.]+)%$/i.test(text)){  //percentage
        this.type = "percentage";
        this.value = +RegExp.$1;
    } else if (/^([+\-]?[\d\.]+)%$/i.test(text)){  //percentage
        this.type = "percentage";
        this.value = +RegExp.$1;
    } else if (/^([+\-]?\d+)$/i.test(text)){  //integer
        this.type = "integer";
        this.value = +RegExp.$1;
    } else if (/^([+\-]?[\d\.]+)$/i.test(text)){  //number
        this.type = "number";
        this.value = +RegExp.$1;
    
    } else if (/^#([a-f0-9]{3,6})/i.test(text)){  //hexcolor
        this.type = "color";
        temp = RegExp.$1;
        if (temp.length == 3){
            this.red    = parseInt(temp.charAt(0)+temp.charAt(0),16);
            this.green  = parseInt(temp.charAt(1)+temp.charAt(1),16);
            this.blue   = parseInt(temp.charAt(2)+temp.charAt(2),16);            
        } else {
            this.red    = parseInt(temp.substring(0,2),16);
            this.green  = parseInt(temp.substring(2,4),16);
            this.blue   = parseInt(temp.substring(4,6),16);            
        }
    } else if (/^rgb\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\)/i.test(text)){ //rgb() color with absolute numbers
        this.type   = "color";
        this.red    = +RegExp.$1;
        this.green  = +RegExp.$2;
        this.blue   = +RegExp.$3;
    } else if (/^rgb\(\s*(\d+)%\s*,\s*(\d+)%\s*,\s*(\d+)%\s*\)/i.test(text)){ //rgb() color with percentages
        this.type   = "color";
        this.red    = +RegExp.$1 * 255 / 100;
        this.green  = +RegExp.$2 * 255 / 100;
        this.blue   = +RegExp.$3 * 255 / 100;
    } else if (/^rgba\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*([\d\.]+)\s*\)/i.test(text)){ //rgba() color with absolute numbers
        this.type   = "color";
        this.red    = +RegExp.$1;
        this.green  = +RegExp.$2;
        this.blue   = +RegExp.$3;
        this.alpha  = +RegExp.$4;
    } else if (/^rgba\(\s*(\d+)%\s*,\s*(\d+)%\s*,\s*(\d+)%\s*,\s*([\d\.]+)\s*\)/i.test(text)){ //rgba() color with percentages
        this.type   = "color";
        this.red    = +RegExp.$1 * 255 / 100;
        this.green  = +RegExp.$2 * 255 / 100;
        this.blue   = +RegExp.$3 * 255 / 100;
        this.alpha  = +RegExp.$4;        
    } else if (/^hsl\(\s*(\d+)\s*,\s*(\d+)%\s*,\s*(\d+)%\s*\)/i.test(text)){ //hsl()
        this.type   = "color";
        this.hue    = +RegExp.$1;
        this.saturation = +RegExp.$2 / 100;
        this.lightness  = +RegExp.$3 / 100;        
    } else if (/^hsla\(\s*(\d+)\s*,\s*(\d+)%\s*,\s*(\d+)%\s*,\s*([\d\.]+)\s*\)/i.test(text)){ //hsla() color with percentages
        this.type   = "color";
        this.hue    = +RegExp.$1;
        this.saturation = +RegExp.$2 / 100;
        this.lightness  = +RegExp.$3 / 100;        
        this.alpha  = +RegExp.$4;        
    } else if (/^url\(["']?([^\)"']+)["']?\)/i.test(text)){ //URI
        this.type   = "uri";
        this.uri    = RegExp.$1;
    } else if (/^["'][^"']*["']/.test(text)){    //string
        this.type   = "string";
        this.value  = eval(text);
    } else if (Colors[text.toLowerCase()]){  //named color
        this.type   = "color";
        temp        = Colors[text.toLowerCase()].substring(1);
        this.red    = parseInt(temp.substring(0,2),16);
        this.green  = parseInt(temp.substring(2,4),16);
        this.blue   = parseInt(temp.substring(4,6),16);         
    } else if (/^[\,\/]$/.test(text)){
        this.type   = "operator";
        this.value  = text;
    } else if (/^[a-z\-\u0080-\uFFFF][a-z0-9\-\u0080-\uFFFF]*$/i.test(text)){
        this.type   = "identifier";
        this.value  = text;
    }

}

PropertyValuePart.prototype = new SyntaxUnit();
PropertyValuePart.prototype.constructor = PropertyValue;

/**
 * Create a new syntax unit based solely on the given token.
 * Convenience method for creating a new syntax unit when
 * it represents a single token instead of multiple.
 * @param {Object} token The token object to represent.
 * @return {parserlib.css.PropertyValuePart} The object representing the token.
 * @static
 * @method fromToken
 */
PropertyValuePart.fromToken = function(token){
    return new PropertyValuePart(token.value, token.startLine, token.startCol);
};
var Pseudos = {
    ":first-letter": 1,
    ":first-line":   1,
    ":before":       1,
    ":after":        1
};

Pseudos.ELEMENT = 1;
Pseudos.CLASS = 2;

Pseudos.isElement = function(pseudo){
    return pseudo.indexOf("::") === 0 || Pseudos[pseudo.toLowerCase()] == Pseudos.ELEMENT;
};
/**
 * Represents an entire single selector, including all parts but not
 * including multiple selectors (those separated by commas).
 * @namespace parserlib.css
 * @class Selector
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 * @param {Array} parts Array of selectors parts making up this selector.
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 */
function Selector(parts, line, col){
    
    SyntaxUnit.call(this, parts.join(" "), line, col, Parser.SELECTOR_TYPE);
    
    /**
     * The parts that make up the selector.
     * @type Array
     * @property parts
     */
    this.parts = parts;
    
    /**
     * The specificity of the selector.
     * @type parserlib.css.Specificity
     * @property specificity
     */
    this.specificity = Specificity.calculate(this);

}

Selector.prototype = new SyntaxUnit();
Selector.prototype.constructor = Selector;

/**
 * Represents a single part of a selector string, meaning a single set of
 * element name and modifiers. This does not include combinators such as
 * spaces, +, >, etc.
 * @namespace parserlib.css
 * @class SelectorPart
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 * @param {String} elementName The element name in the selector or null
 *      if there is no element name.
 * @param {Array} modifiers Array of individual modifiers for the element.
 *      May be empty if there are none.
 * @param {String} text The text representation of the unit. 
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 */
function SelectorPart(elementName, modifiers, text, line, col){
    
    SyntaxUnit.call(this, text, line, col, Parser.SELECTOR_PART_TYPE);

    /**
     * The tag name of the element to which this part
     * of the selector affects.
     * @type String
     * @property elementName
     */
    this.elementName = elementName;
    
    /**
     * The parts that come after the element name, such as class names, IDs,
     * pseudo classes/elements, etc.
     * @type Array
     * @property modifiers
     */
    this.modifiers = modifiers;

}

SelectorPart.prototype = new SyntaxUnit();
SelectorPart.prototype.constructor = SelectorPart;

/**
 * Represents a selector modifier string, meaning a class name, element name,
 * element ID, pseudo rule, etc.
 * @namespace parserlib.css
 * @class SelectorSubPart
 * @extends parserlib.util.SyntaxUnit
 * @constructor
 * @param {String} text The text representation of the unit. 
 * @param {String} type The type of selector modifier.
 * @param {int} line The line of text on which the unit resides.
 * @param {int} col The column of text on which the unit resides.
 */
function SelectorSubPart(text, type, line, col){
    
    SyntaxUnit.call(this, text, line, col, Parser.SELECTOR_SUB_PART_TYPE);

    /**
     * The type of modifier.
     * @type String
     * @property type
     */
    this.type = type;
    
    /**
     * Some subparts have arguments, this represents them.
     * @type Array
     * @property args
     */
    this.args = [];

}

SelectorSubPart.prototype = new SyntaxUnit();
SelectorSubPart.prototype.constructor = SelectorSubPart;

/**
 * Represents a selector's specificity.
 * @namespace parserlib.css
 * @class Specificity
 * @constructor
 * @param {int} a Should be 1 for inline styles, zero for stylesheet styles
 * @param {int} b Number of ID selectors
 * @param {int} c Number of classes and pseudo classes
 * @param {int} d Number of element names and pseudo elements
 */
function Specificity(a, b, c, d){
    this.a = a;
    this.b = b;
    this.c = c;
    this.d = d;
}

Specificity.prototype = {
    constructor: Specificity,
    
    /**
     * Compare this specificity to another.
     * @param {Specificity} other The other specificity to compare to.
     * @return {int} -1 if the other specificity is larger, 1 if smaller, 0 if equal.
     * @method compare
     */
    compare: function(other){
        var comps = ["a", "b", "c", "d"],
            i, len;
            
        for (i=0, len=comps.length; i < len; i++){
            if (this[comps[i]] < other[comps[i]]){
                return -1;
            } else if (this[comps[i]] > other[comps[i]]){
                return 1;
            }
        }
        
        return 0;
    },
    
    /**
     * Creates a numeric value for the specificity.
     * @return {int} The numeric value for the specificity.
     * @method valueOf
     */
    valueOf: function(){
        return (this.a * 1000) + (this.b * 100) + (this.c * 10) + this.d;
    },
    
    /**
     * Returns a string representation for specificity.
     * @return {String} The string representation of specificity.
     * @method toString
     */
    toString: function(){
        return this.a + "," + this.b + "," + this.c + "," + this.d;
    }

};

/**
 * Calculates the specificity of the given selector.
 * @param {parserlib.css.Selector} The selector to calculate specificity for.
 * @return {parserlib.css.Specificity} The specificity of the selector.
 * @static
 * @method calculate
 */
Specificity.calculate = function(selector){

    var i, len,
        b=0, c=0, d=0;
        
    function updateValues(part){
    
        var i, j, len, num,
            modifier;
    
        if (part.elementName && part.text.charAt(part.text.length-1) != "*") {
            d++;
        }    
    
        for (i=0, len=part.modifiers.length; i < len; i++){
            modifier = part.modifiers[i];
            switch(modifier.type){
                case "class":
                case "attribute":
                    c++;
                    break;
                    
                case "id":
                    b++;
                    break;
                    
                case "pseudo":
                    if (Pseudos.isElement(modifier.text)){
                        d++;
                    } else {
                        c++;
                    }                    
                    break;
                    
                case "not":
                    for (j=0, num=modifier.args.length; j < num; j++){
                        updateValues(modifier.args[j]);
                    }
            }    
         }
    }
    
    for (i=0, len=selector.parts.length; i < len; i++){
        part = selector.parts[i];
        
        if (part instanceof SelectorPart){
            updateValues(part);                
        }
    }
    
    return new Specificity(0, b, c, d);
};


var h = /^[0-9a-fA-F]$/,
    nonascii = /^[\u0080-\uFFFF]$/,
    nl = /\n|\r\n|\r|\f/;

//-----------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------


function isHexDigit(c){
    return c != null && h.test(c);
}

function isDigit(c){
    return c != null && /\d/.test(c);
}

function isWhitespace(c){
    return c != null && /\s/.test(c);
}

function isNewLine(c){
    return c != null && nl.test(c);
}

function isNameStart(c){
    return c != null && (/[a-z_\u0080-\uFFFF\\]/i.test(c));
}

function isNameChar(c){
    return c != null && (isNameStart(c) || /[0-9\-\\]/.test(c));
}

function isIdentStart(c){
    return c != null && (isNameStart(c) || /\-\\/.test(c));
}

function mix(receiver, supplier){
	for (var prop in supplier){
		if (supplier.hasOwnProperty(prop)){
			receiver[prop] = supplier[prop];
		}
	}
	return receiver;
}

//-----------------------------------------------------------------------------
// CSS Token Stream
//-----------------------------------------------------------------------------


/**
 * A token stream that produces CSS tokens.
 * @param {String|Reader} input The source of text to tokenize.
 * @constructor
 * @class TokenStream
 * @namespace parserlib.css
 */
function TokenStream(input){
	TokenStreamBase.call(this, input, Tokens);
}

TokenStream.prototype = mix(new TokenStreamBase(), {

    /**
     * Overrides the TokenStreamBase method of the same name
     * to produce CSS tokens.
     * @param {variant} channel The name of the channel to use
     *      for the next token.
     * @return {Object} A token object representing the next token.
     * @method _getToken
     * @private
     */
    _getToken: function(channel){

        var c,
            reader = this._reader,
            token   = null,
            startLine   = reader.getLine(),
            startCol    = reader.getCol();

        c = reader.read();


        while(c){
            switch(c){

                /*
                 * Potential tokens:
                 * - COMMENT
                 * - SLASH
                 * - CHAR
                 */
                case "/":

                    if(reader.peek() == "*"){
                        token = this.commentToken(c, startLine, startCol);
                    } else {
                        token = this.charToken(c, startLine, startCol);
                    }
                    break;

                /*
                 * Potential tokens:
                 * - DASHMATCH
                 * - INCLUDES
                 * - PREFIXMATCH
                 * - SUFFIXMATCH
                 * - SUBSTRINGMATCH
                 * - CHAR
                 */
                case "|":
                case "~":
                case "^":
                case "$":
                case "*":
                    if(reader.peek() == "="){
                        token = this.comparisonToken(c, startLine, startCol);
                    } else {
                        token = this.charToken(c, startLine, startCol);
                    }
                    break;

                /*
                 * Potential tokens:
                 * - STRING
                 * - INVALID
                 */
                case "\"":
                case "'":
                    token = this.stringToken(c, startLine, startCol);
                    break;

                /*
                 * Potential tokens:
                 * - HASH
                 * - CHAR
                 */
                case "#":
                    if (isNameChar(reader.peek())){
                        token = this.hashToken(c, startLine, startCol);
                    } else {
                        token = this.charToken(c, startLine, startCol);
                    }
                    break;

                /*
                 * Potential tokens:
                 * - DOT
                 * - NUMBER
                 * - DIMENSION
                 * - PERCENTAGE
                 */
                case ".":
                    if (isDigit(reader.peek())){
                        token = this.numberToken(c, startLine, startCol);
                    } else {
                        token = this.charToken(c, startLine, startCol);
                    }
                    break;

                /*
                 * Potential tokens:
                 * - CDC
                 * - MINUS
                 * - NUMBER
                 * - DIMENSION
                 * - PERCENTAGE
                 */
                case "-":
                    if (reader.peek() == "-"){  //could be closing HTML-style comment
                        token = this.htmlCommentEndToken(c, startLine, startCol);
                    } else if (isNameStart(reader.peek())){
                        token = this.identOrFunctionToken(c, startLine, startCol);
                    } else {
                        token = this.charToken(c, startLine, startCol);
                    }
                    break;

                /*
                 * Potential tokens:
                 * - IMPORTANT_SYM
                 * - CHAR
                 */
                case "!":
                    token = this.importantToken(c, startLine, startCol);
                    break;

                /*
                 * Any at-keyword or CHAR
                 */
                case "@":
                    token = this.atRuleToken(c, startLine, startCol);
                    break;

                /*
                 * Potential tokens:
                 * - NOT
                 * - CHAR
                 */
                case ":":
                    token = this.notToken(c, startLine, startCol);
                    break;

                /*
                 * Potential tokens:
                 * - CDO
                 * - CHAR
                 */
                case "<":
                    token = this.htmlCommentStartToken(c, startLine, startCol);
                    break;

                /*
                 * Potential tokens:
                 * - UNICODE_RANGE
                 * - URL
                 * - CHAR
                 */
                case "U":
                case "u":
                    if (reader.peek() == "+"){
                        token = this.unicodeRangeToken(c, startLine, startCol);
                        break;
                    }
                    /*falls through*/

                default:

                    /*
                     * Potential tokens:
                     * - NUMBER
                     * - DIMENSION
                     * - LENGTH
                     * - FREQ
                     * - TIME
                     * - EMS
                     * - EXS
                     * - ANGLE
                     */
                    if (isDigit(c)){
                        token = this.numberToken(c, startLine, startCol);
                    } else

                    /*
                     * Potential tokens:
                     * - S
                     */
                    if (isWhitespace(c)){
                        token = this.whitespaceToken(c, startLine, startCol);
                    } else

                    /*
                     * Potential tokens:
                     * - IDENT
                     */
                    if (isIdentStart(c)){
                        token = this.identOrFunctionToken(c, startLine, startCol);
                    } else

                    /*
                     * Potential tokens:
                     * - CHAR
                     * - PLUS
                     */
                    {
                        token = this.charToken(c, startLine, startCol);
                    }






            }

            //make sure this token is wanted
            //TODO: check channel
            break;

            c = reader.read();
        }

        if (!token && c == null){
            token = this.createToken(Tokens.EOF,null,startLine,startCol);
        }

        return token;
    },

    //-------------------------------------------------------------------------
    // Methods to create tokens
    //-------------------------------------------------------------------------

    /**
     * Produces a token based on available data and the current
     * reader position information. This method is called by other
     * private methods to create tokens and is never called directly.
     * @param {int} tt The token type.
     * @param {String} value The text value of the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @param {Object} options (Optional) Specifies a channel property
     *      to indicate that a different channel should be scanned
     *      and/or a hide property indicating that the token should
     *      be hidden.
     * @return {Object} A token object.
     * @method createToken
     */
    createToken: function(tt, value, startLine, startCol, options){
        var reader = this._reader;
        options = options || {};

        return {
            value:      value,
            type:       tt,
            channel:    options.channel,
            hide:       options.hide || false,
            startLine:  startLine,
            startCol:   startCol,
            endLine:    reader.getLine(),
            endCol:     reader.getCol()
        };
    },

    //-------------------------------------------------------------------------
    // Methods to create specific tokens
    //-------------------------------------------------------------------------

    /**
     * Produces a token for any at-rule. If the at-rule is unknown, then
     * the token is for a single "@" character.
     * @param {String} first The first character for the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method atRuleToken
     */
    atRuleToken: function(first, startLine, startCol){
        var rule    = first,
            reader  = this._reader,
            tt      = Tokens.CHAR,
            valid   = false,
            ident,
            c;

        /*
         * First, mark where we are. There are only four @ rules,
         * so anything else is really just an invalid token.
         * Basically, if this doesn't match one of the known @
         * rules, just return '@' as an unknown token and allow
         * parsing to continue after that point.
         */
        reader.mark();

        //try to find the at-keyword
        ident = this.readName();
        rule = first + ident;
        tt = Tokens.type(rule.toLowerCase());

        //if it's not valid, use the first character only and reset the reader
        if (tt == Tokens.CHAR || tt == Tokens.UNKNOWN){
            tt = Tokens.CHAR;
            rule = first;
            reader.reset();
        }

        return this.createToken(tt, rule, startLine, startCol);
    },

    /**
     * Produces a character token based on the given character
     * and location in the stream. If there's a special (non-standard)
     * token name, this is used; otherwise CHAR is used.
     * @param {String} c The character for the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method charToken
     */
    charToken: function(c, startLine, startCol){
        var tt = Tokens.type(c);

        if (tt == -1){
            tt = Tokens.CHAR;
        }

        return this.createToken(tt, c, startLine, startCol);
    },

    /**
     * Produces a character token based on the given character
     * and location in the stream. If there's a special (non-standard)
     * token name, this is used; otherwise CHAR is used.
     * @param {String} first The first character for the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method commentToken
     */
    commentToken: function(first, startLine, startCol){
        var reader  = this._reader,
            comment = this.readComment(first);

        return this.createToken(Tokens.COMMENT, comment, startLine, startCol);
    },

    /**
     * Produces a comparison token based on the given character
     * and location in the stream. The next character must be
     * read and is already known to be an equals sign.
     * @param {String} c The character for the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method comparisonToken
     */
    comparisonToken: function(c, startLine, startCol){
        var reader  = this._reader,
            comparison  = c + reader.read(),
            tt      = Tokens.type(comparison) || Tokens.CHAR;

        return this.createToken(tt, comparison, startLine, startCol);
    },

    /**
     * Produces a hash token based on the specified information. The
     * first character provided is the pound sign (#) and then this
     * method reads a name afterward.
     * @param {String} first The first character (#) in the hash name.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method hashToken
     */
    hashToken: function(first, startLine, startCol){
        var reader  = this._reader,
            name    = this.readName(first);

        return this.createToken(Tokens.HASH, name, startLine, startCol);
    },

    /**
     * Produces a CDO or CHAR token based on the specified information. The
     * first character is provided and the rest is read by the function to determine
     * the correct token to create.
     * @param {String} first The first character in the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method htmlCommentStartToken
     */
    htmlCommentStartToken: function(first, startLine, startCol){
        var reader      = this._reader,
            text        = first;

        reader.mark();
        text += reader.readCount(3);

        if (text == "<!--"){
            return this.createToken(Tokens.CDO, text, startLine, startCol);
        } else {
            reader.reset();
            return this.charToken(first, startLine, startCol);
        }
    },

    /**
     * Produces a CDC or CHAR token based on the specified information. The
     * first character is provided and the rest is read by the function to determine
     * the correct token to create.
     * @param {String} first The first character in the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method htmlCommentEndToken
     */
    htmlCommentEndToken: function(first, startLine, startCol){
        var reader      = this._reader,
            text        = first;

        reader.mark();
        text += reader.readCount(2);

        if (text == "-->"){
            return this.createToken(Tokens.CDC, text, startLine, startCol);
        } else {
            reader.reset();
            return this.charToken(first, startLine, startCol);
        }
    },

    /**
     * Produces an IDENT or FUNCTION token based on the specified information. The
     * first character is provided and the rest is read by the function to determine
     * the correct token to create.
     * @param {String} first The first character in the identifier.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method identOrFunctionToken
     */
    identOrFunctionToken: function(first, startLine, startCol){
        var reader  = this._reader,
            ident   = this.readName(first),
            tt      = Tokens.IDENT;

        //if there's a left paren immediately after, it's a URI or function
        if (reader.peek() == "("){
            ident += reader.read();
            if (ident.toLowerCase() == "url("){
                tt = Tokens.URI;
                ident = this.readURI(ident);

                //didn't find a valid URL or there's no closing paren
                if (ident.toLowerCase() == "url("){
                    tt = Tokens.FUNCTION;
                }
            } else {
                tt = Tokens.FUNCTION;
            }
        } else if (reader.peek() == ":"){  //might be an IE function

            //IE-specific functions always being with progid:
            if (ident.toLowerCase() == "progid"){
                ident += reader.readTo("(");
                tt = Tokens.IE_FUNCTION;
            }
        }

        return this.createToken(tt, ident, startLine, startCol);
    },

    /**
     * Produces an IMPORTANT_SYM or CHAR token based on the specified information. The
     * first character is provided and the rest is read by the function to determine
     * the correct token to create.
     * @param {String} first The first character in the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method importantToken
     */
    importantToken: function(first, startLine, startCol){
        var reader      = this._reader,
            important   = first,
            tt          = Tokens.CHAR,
            temp,
            c;

        reader.mark();
        c = reader.read();

        while(c){

            //there can be a comment in here
            if (c == "/"){

                //if the next character isn't a star, then this isn't a valid !important token
                if (reader.peek() != "*"){
                    break;
                } else {
                    temp = this.readComment(c);
                    if (temp == ""){    //broken!
                        break;
                    }
                }
            } else if (isWhitespace(c)){
                important += c + this.readWhitespace();
            } else if (/i/i.test(c)){
                temp = reader.readCount(8);
                if (/mportant/i.test(temp)){
                    important += c + temp;
                    tt = Tokens.IMPORTANT_SYM;

                }
                break;  //we're done
            } else {
                break;
            }

            c = reader.read();
        }

        if (tt == Tokens.CHAR){
            reader.reset();
            return this.charToken(first, startLine, startCol);
        } else {
            return this.createToken(tt, important, startLine, startCol);
        }


    },

    /**
     * Produces a NOT or CHAR token based on the specified information. The
     * first character is provided and the rest is read by the function to determine
     * the correct token to create.
     * @param {String} first The first character in the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method notToken
     */
    notToken: function(first, startLine, startCol){
        var reader      = this._reader,
            text        = first;

        reader.mark();
        text += reader.readCount(4);

        if (text.toLowerCase() == ":not("){
            return this.createToken(Tokens.NOT, text, startLine, startCol);
        } else {
            reader.reset();
            return this.charToken(first, startLine, startCol);
        }
    },

    /**
     * Produces a number token based on the given character
     * and location in the stream. This may return a token of
     * NUMBER, EMS, EXS, LENGTH, ANGLE, TIME, FREQ, DIMENSION,
     * or PERCENTAGE.
     * @param {String} first The first character for the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method numberToken
     */
    numberToken: function(first, startLine, startCol){
        var reader  = this._reader,
            value   = this.readNumber(first),
            ident,
            tt      = Tokens.NUMBER,
            c       = reader.peek();

        if (isIdentStart(c)){
            ident = this.readName(reader.read());
            value += ident;

            if (/^em$|^ex$|^px$|^gd$|^rem$|^vw$|^vh$|^vm$|^ch$|^cm$|^mm$|^in$|^pt$|^pc$/i.test(ident)){
                tt = Tokens.LENGTH;
            } else if (/^deg|^rad$|^grad$/i.test(ident)){
                tt = Tokens.ANGLE;
            } else if (/^ms$|^s$/i.test(ident)){
                tt = Tokens.TIME;
            } else if (/^hz$|^khz$/i.test(ident)){
                tt = Tokens.FREQ;
            } else if (/^dpi$|^dpcm$/i.test(ident)){
                tt = Tokens.RESOLUTION;
            } else {
                tt = Tokens.DIMENSION;
            }

        } else if (c == "%"){
            value += reader.read();
            tt = Tokens.PERCENTAGE;
        }

        return this.createToken(tt, value, startLine, startCol);
    },

    /**
     * Produces a string token based on the given character
     * and location in the stream. Since strings may be indicated
     * by single or double quotes, a failure to match starting
     * and ending quotes results in an INVALID token being generated.
     * The first character in the string is passed in and then
     * the rest are read up to and including the final quotation mark.
     * @param {String} first The first character in the string.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method stringToken
     */
    stringToken: function(first, startLine, startCol){
        var delim   = first,
            string  = first,
            reader  = this._reader,
            prev    = first,
            tt      = Tokens.STRING,
            c       = reader.read();

        while(c){
            string += c;

            //if the delimiter is found with an escapement, we're done.
            if (c == delim && prev != "\\"){
                break;
            }

            //if there's a newline without an escapement, it's an invalid string
            if (isNewLine(reader.peek()) && c != "\\"){
                tt = Tokens.INVALID;
                break;
            }

            //save previous and get next
            prev = c;
            c = reader.read();
        }

        //if c is null, that means we're out of input and the string was never closed
        if (c == null){
            tt = Tokens.INVALID;
        }

        return this.createToken(tt, string, startLine, startCol);
    },

    unicodeRangeToken: function(first, startLine, startCol){
        var reader  = this._reader,
            value   = first,
            temp,
            tt      = Tokens.CHAR;

        //then it should be a unicode range
        if (reader.peek() == "+"){
            reader.mark();
            value += reader.read();
            value += this.readUnicodeRangePart(true);

            //ensure there's an actual unicode range here
            if (value.length == 2){
                reader.reset();
            } else {

                tt = Tokens.UNICODE_RANGE;

                //if there's a ? in the first part, there can't be a second part
                if (value.indexOf("?") == -1){

                    if (reader.peek() == "-"){
                        reader.mark();
                        temp = reader.read();
                        temp += this.readUnicodeRangePart(false);

                        //if there's not another value, back up and just take the first
                        if (temp.length == 1){
                            reader.reset();
                        } else {
                            value += temp;
                        }
                    }

                }
            }
        }

        return this.createToken(tt, value, startLine, startCol);
    },

    /**
     * Produces a S token based on the specified information. Since whitespace
     * may have multiple characters, this consumes all whitespace characters
     * into a single token.
     * @param {String} first The first character in the token.
     * @param {int} startLine The beginning line for the character.
     * @param {int} startCol The beginning column for the character.
     * @return {Object} A token object.
     * @method whitespaceToken
     */
    whitespaceToken: function(first, startLine, startCol){
        var reader  = this._reader,
            value   = first + this.readWhitespace();
        return this.createToken(Tokens.S, value, startLine, startCol);
    },




    //-------------------------------------------------------------------------
    // Methods to read values from the string stream
    //-------------------------------------------------------------------------

    readUnicodeRangePart: function(allowQuestionMark){
        var reader  = this._reader,
            part = "",
            c       = reader.peek();

        //first read hex digits
        while(isHexDigit(c) && part.length < 6){
            reader.read();
            part += c;
            c = reader.peek();
        }

        //then read question marks if allowed
        if (allowQuestionMark){
            while(c == "?" && part.length < 6){
                reader.read();
                part += c;
                c = reader.peek();
            }
        }

        //there can't be any other characters after this point

        return part;
    },

    readWhitespace: function(){
        var reader  = this._reader,
            whitespace = "",
            c       = reader.peek();

        while(isWhitespace(c)){
            reader.read();
            whitespace += c;
            c = reader.peek();
        }

        return whitespace;
    },
    readNumber: function(first){
        var reader  = this._reader,
            number  = first,
            hasDot  = (first == "."),
            c       = reader.peek();


        while(c){
            if (isDigit(c)){
                number += reader.read();
            } else if (c == "."){
                if (hasDot){
                    break;
                } else {
                    hasDot = true;
                    number += reader.read();
                }
            } else {
                break;
            }

            c = reader.peek();
        }

        return number;
    },
    readString: function(){
        var reader  = this._reader,
            delim   = reader.read(),
            string  = delim,
            prev    = delim,
            c       = reader.peek();

        while(c){
            c = reader.read();
            string += c;

            //if the delimiter is found with an escapement, we're done.
            if (c == delim && prev != "\\"){
                break;
            }

            //if there's a newline without an escapement, it's an invalid string
            if (isNewLine(reader.peek()) && c != "\\"){
                string = "";
                break;
            }

            //save previous and get next
            prev = c;
            c = reader.peek();
        }

        //if c is null, that means we're out of input and the string was never closed
        if (c == null){
            string = "";
        }

        return string;
    },
    readURI: function(first){
        var reader  = this._reader,
            uri     = first,
            inner   = "",
            c       = reader.peek();

        reader.mark();

        //skip whitespace before
        while(c && isWhitespace(c)){
            reader.read();
            c = reader.peek();
        }

        //it's a string
        if (c == "'" || c == "\""){
            inner = this.readString();
        } else {
            inner = this.readURL();
        }

        c = reader.peek();

        //skip whitespace after
        while(c && isWhitespace(c)){
            reader.read();
            c = reader.peek();
        }

        //if there was no inner value or the next character isn't closing paren, it's not a URI
        if (inner == "" || c != ")"){
            uri = first;
            reader.reset();
        } else {
            uri += inner + reader.read();
        }

        return uri;
    },
    readURL: function(){
        var reader  = this._reader,
            url     = "",
            c       = reader.peek();

        //TODO: Check for escape and nonascii
        while (/^[!#$%&\\*-~]$/.test(c)){
            url += reader.read();
            c = reader.peek();
        }

        return url;

    },
    readName: function(first){
        var reader  = this._reader,
            ident   = first || "",
            c       = reader.peek();

        while(true){
            if (c == "\\"){
                ident += this.readEscape(reader.read());
                c = reader.peek();
            } else if(c && isNameChar(c)){
                ident += reader.read();
                c = reader.peek();
            } else {
                break;
            }
        }

        return ident;
    },
    
    readEscape: function(first){
        var reader  = this._reader,
            cssEscape = first || "",
            i       = 0,
            c       = reader.peek();    
    
        if (isHexDigit(c)){
            do {
                cssEscape += reader.read();
                c = reader.peek();
            } while(c && isHexDigit(c) && ++i < 6);
        }
        
        if (cssEscape.length == 3 && /\s/.test(c) ||
            cssEscape.length == 7 || cssEscape.length == 1){
                reader.read();
        } else {
            c = "";
        }
        
        return cssEscape + c;
    },
    
    readComment: function(first){
        var reader  = this._reader,
            comment = first || "",
            c       = reader.read();

        if (c == "*"){
            while(c){
                comment += c;

                //look for end of comment
                if (comment.length > 2 && c == "*" && reader.peek() == "/"){
                    comment += reader.read();
                    break;
                }

                c = reader.read();
            }

            return comment;
        } else {
            return "";
        }

    }
});

var Tokens  = [

    /*
     * The following token names are defined in CSS3 Grammar: http://www.w3.org/TR/css3-syntax/#lexical
     */
     
    //HTML-style comments
    { name: "CDO"},
    { name: "CDC"},

    //ignorables
    { name: "S", whitespace: true/*, channel: "ws"*/},
    { name: "COMMENT", comment: true, hide: true, channel: "comment" },
        
    //attribute equality
    { name: "INCLUDES", text: "~="},
    { name: "DASHMATCH", text: "|="},
    { name: "PREFIXMATCH", text: "^="},
    { name: "SUFFIXMATCH", text: "$="},
    { name: "SUBSTRINGMATCH", text: "*="},
        
    //identifier types
    { name: "STRING"},     
    { name: "IDENT"},
    { name: "HASH"},

    //at-keywords
    { name: "IMPORT_SYM", text: "@import"},
    { name: "PAGE_SYM", text: "@page"},
    { name: "MEDIA_SYM", text: "@media"},
    { name: "FONT_FACE_SYM", text: "@font-face"},
    { name: "CHARSET_SYM", text: "@charset"},
    { name: "NAMESPACE_SYM", text: "@namespace"},
    //{ name: "ATKEYWORD"},
    
    //CSS3 animations
    { name: "KEYFRAMES_SYM", text: [ "@keyframes", "@-webkit-keyframes", "@-moz-keyframes" ] },

    //important symbol
    { name: "IMPORTANT_SYM"},

    //measurements
    { name: "LENGTH"},
    { name: "ANGLE"},
    { name: "TIME"},
    { name: "FREQ"},
    { name: "DIMENSION"},
    { name: "PERCENTAGE"},
    { name: "NUMBER"},
    
    //functions
    { name: "URI"},
    { name: "FUNCTION"},
    
    //Unicode ranges
    { name: "UNICODE_RANGE"},
    
    /*
     * The following token names are defined in CSS3 Selectors: http://www.w3.org/TR/css3-selectors/#selector-syntax
     */    
    
    //invalid string
    { name: "INVALID"},
    
    //combinators
    { name: "PLUS", text: "+" },
    { name: "GREATER", text: ">"},
    { name: "COMMA", text: ","},
    { name: "TILDE", text: "~"},
    
    //modifier
    { name: "NOT"},        
    
    /*
     * Defined in CSS3 Paged Media
     */
    { name: "TOPLEFTCORNER_SYM", text: "@top-left-corner"},
    { name: "TOPLEFT_SYM", text: "@top-left"},
    { name: "TOPCENTER_SYM", text: "@top-center"},
    { name: "TOPRIGHT_SYM", text: "@top-right"},
    { name: "TOPRIGHTCORNER_SYM", text: "@top-right-corner"},
    { name: "BOTTOMLEFTCORNER_SYM", text: "@bottom-left-corner"},
    { name: "BOTTOMLEFT_SYM", text: "@bottom-left"},
    { name: "BOTTOMCENTER_SYM", text: "@bottom-center"},
    { name: "BOTTOMRIGHT_SYM", text: "@bottom-right"},
    { name: "BOTTOMRIGHTCORNER_SYM", text: "@bottom-right-corner"},
    { name: "LEFTTOP_SYM", text: "@left-top"},
    { name: "LEFTMIDDLE_SYM", text: "@left-middle"},
    { name: "LEFTBOTTOM_SYM", text: "@left-bottom"},
    { name: "RIGHTTOP_SYM", text: "@right-top"},
    { name: "RIGHTMIDDLE_SYM", text: "@right-middle"},
    { name: "RIGHTBOTTOM_SYM", text: "@right-bottom"},

    /*
     * The following token names are defined in CSS3 Media Queries: http://www.w3.org/TR/css3-mediaqueries/#syntax
     */
    /*{ name: "MEDIA_ONLY", state: "media"},
    { name: "MEDIA_NOT", state: "media"},
    { name: "MEDIA_AND", state: "media"},*/
    { name: "RESOLUTION", state: "media"},

    /*
     * The following token names are not defined in any CSS specification but are used by the lexer.
     */
    
    //not a real token, but useful for stupid IE filters
    { name: "IE_FUNCTION" },

    //part of CSS3 grammar but not the Flex code
    { name: "CHAR" },
    
    //TODO: Needed?
    //Not defined as tokens, but might as well be
    {
        name: "PIPE",
        text: "|"
    },
    {
        name: "SLASH",
        text: "/"
    },
    {
        name: "MINUS",
        text: "-"
    },
    {
        name: "STAR",
        text: "*"
    },

    {
        name: "LBRACE",
        text: "{"
    },   
    {
        name: "RBRACE",
        text: "}"
    },      
    {
        name: "LBRACKET",
        text: "["
    },   
    {
        name: "RBRACKET",
        text: "]"
    },    
    {
        name: "EQUALS",
        text: "="
    },
    {
        name: "COLON",
        text: ":"
    },    
    {
        name: "SEMICOLON",
        text: ";"
    },    
 
    {
        name: "LPAREN",
        text: "("
    },   
    {
        name: "RPAREN",
        text: ")"
    },     
    {
        name: "DOT",
        text: "."
    }
];

(function(){

    var nameMap = [],
        typeMap = {};
    
    Tokens.UNKNOWN = -1;
    Tokens.unshift({name:"EOF"});
    for (var i=0, len = Tokens.length; i < len; i++){
        nameMap.push(Tokens[i].name);
        Tokens[Tokens[i].name] = i;
        if (Tokens[i].text){
            if (Tokens[i].text instanceof Array){
                for (var j=0; j < Tokens[i].text.length; j++){
                    typeMap[Tokens[i].text[j]] = i;
                }
            } else {
                typeMap[Tokens[i].text] = i;
            }
        }
    }
    
    Tokens.name = function(tt){
        return nameMap[tt];
    };
    
    Tokens.type = function(c){
        return typeMap[c] || -1;
    };

})();



/**
 * Type to use when a validation error occurs.
 * @class ValidationError
 * @namespace parserlib.util
 * @constructor
 * @param {String} message The error message.
 * @param {int} line The line at which the error occurred.
 * @param {int} col The column at which the error occurred.
 */
function ValidationError(message, line, col){

    /**
     * The column at which the error occurred.
     * @type int
     * @property col
     */
    this.col = col;

    /**
     * The line at which the error occurred.
     * @type int
     * @property line
     */
    this.line = line;

    /**
     * The text representation of the unit.
     * @type String
     * @property text
     */
    this.message = message;

}

//inherit from Error
ValidationError.prototype = new Error();

parserlib.css = {
Colors              :Colors,    
Combinator          :Combinator,                
Parser              :Parser,
PropertyName        :PropertyName,
PropertyValue       :PropertyValue,
PropertyValuePart   :PropertyValuePart,
MediaFeature        :MediaFeature,
MediaQuery          :MediaQuery,
Selector            :Selector,
SelectorPart        :SelectorPart,
SelectorSubPart     :SelectorSubPart,
Specificity         :Specificity,
TokenStream         :TokenStream,
Tokens              :Tokens,
ValidationError     :ValidationError
};
})();



/************************************************************************
 *  src/impl/URL.js
 ************************************************************************/

//@line 1 "src/impl/URL.js"
function URL(url) {
    if (!url) return Object.create(URL.prototype);
    // Can't use String.trim() since it defines whitespace differently than HTML
    this.url = url.replace(/^[ \t\n\r\f]+|[ \t\n\r\f]+$/g, "");

    // See http://tools.ietf.org/html/rfc3986#appendix-B
    var match = URL.pattern.exec(this.url);
    if (match) {
        if (match[2]) this.scheme = match[2];
        if (match[4]) {
            // XXX ignoring userinfo before the hostname
            if (match[4].match(URL.portPattern)) {
                var pos = S.lastIndexOf(match[4], ":");
                this.host = substring(match[4], 0, pos);
                this.port = substring(match[4], pos+1);
            }
            else {
                this.host = match[4];
            }
        }
        if (match[5]) this.path = match[5];
        if (match[6]) this.query = match[7];
        if (match[8]) this.fragment = match[9];
    }
}

URL.pattern = /^(([^:\/?#]+):)?(\/\/([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?$/;
URL.portPattern = /:\d+$/
URL.authorityPattern = /^[^:\/?#]+:\/\//;
URL.hierarchyPattern = /^[^:\/?#]+:\//;

// Return a percentEncoded version of s.
// S should be a single-character string
// XXX: needs to do utf-8 encoding?
URL.percentEncode = function percentEncode(s) {
    var c = charCodeAt(s, 0);
    if (c < 256) return "%" + c.toString(16);
    else throw Error("can't percent-encode codepoints > 255 yet");
};

URL.prototype = {
    constructor: URL,

    // XXX: not sure if this is the precise definition of absolute
    isAbsolute: function() { return !!this.scheme; },
    isAuthorityBased: function() {
        return URL.authorityPattern.test(this.url);
    },
    isHierarchical: function() {
        return URL.hierarchyPattern.test(this.url);
    },

    toString: function() {
        var s = "";
        if (this.scheme !== undefined) s += this.scheme + ":";
        if (this.host !== undefined) s += "//" + this.host;
        if (this.port !== undefined) s += ":" + this.port;
        if (this.path !== undefined) s += this.path;
        if (this.query !== undefined) s += "?" + this.query;
        if (this.fragment !== undefined) s += "#" + this.fragment;
        return s;
    },

    // See: http://tools.ietf.org/html/rfc3986#section-5.2
    resolve: function(relative) {
        var base = this;            // The base url we're resolving against
        var r = new URL(relative);  // The relative reference url to resolve
        var t = new URL();          // The absolute target url we will return

        if (r.scheme !== undefined) {
            t.scheme = r.scheme;
            t.host = r.host;
            t.port = r.port;
            t.path = remove_dot_segments(r.path);
            t.query = r.query;
        }
        else {
            t.scheme = base.scheme;
            if (r.host !== undefined) {
                t.host = r.host;
                t.port = r.port;
                t.path = remove_dot_segments(r.path);
                t.query = r.query;
            }
            else {
                t.host = base.host;
                t.port = base.port;
                if (!r.path) { // undefined or empty
                    t.path = base.path;
                    if (r.query !== undefined)
                        t.query = r.query;
                    else
                        t.query = base.query;
                }
                else {
                    if (r.path.charAt(0) === "/") {
                        t.path = remove_dot_segments(r.path);
                    }
                    else {
                        t.path = merge(base.path, r.path);
                        t.path = remove_dot_segments(t.path);
                    }
                    t.query = r.query;
                }
            }
        }
        t.fragment = r.fragment;

        return t.toString();


        function merge(basepath, refpath) {
            if (base.host !== undefined && !base.path)
                return "/" + refpath;

            var lastslash = basepath.lastIndexOf("/");
            if (lastslash === -1)
                return refpath;
            else
                return basepath.substring(0, lastslash+1) + refpath;
        }

        function remove_dot_segments(path) {
            if (!path) return path;  // For "" or undefined

            var output = "";
            while(path.length > 0) {
                if (path === "." || path === "..") {
                    path = "";
                    break;
                }

                var twochars = path.substring(0,2);
                var threechars = path.substring(0,3);
                var fourchars = path.substring(0,4);
                if (threechars === "../") {
                    path = path.substring(3);
                }
                else if (twochars === "./") {
                    path = path.substring(2);
                }
                else if (threechars === "/./") {
                    path = "/" + path.substring(3);
                }
                else if (twochars === "/." && path.length === 2) {
                    path = "/";
                }
                else if (fourchars === "/../" ||
                         (threechars === "/.." && path.length === 3)) {
                    path = "/" + path.substring(4);

                    output = output.replace(/\/?[^\/]*$/, "");
                }
                else {
                    var segment = path.match(/(\/?([^\/]*))/)[0];
                    output += segment;
                    path = path.substring(segment.length);
                }
            }


            return output;
        }
    },
};

URL.urlObjectIndex = 0;
URL.createObjectURL = function (blob) {
    URL.objects.push({ data: blob, key: URL.urlObjectIndex });
    return URL.urlObjectIndex++;
};

URL.revokeObjectURL = function (url) {
    for (var index = 0; index < URL.objects.length; index++) {
        if (url === URL.objects[index].key) {
            URL.objects.splice(index, 1);
            break;
        }
    }
};

URL.objects = [];


/************************************************************************
 *  src/impl/URLDecompositionAttributes.js
 ************************************************************************/

//@line 1 "src/impl/URLDecompositionAttributes.js"
// This is an abstract superclass for Location, HTMLAnchorElement and
// other types that have the standard complement of "URL decomposition
// IDL attributes".
// Subclasses must define getInput() and setOutput() methods.
// The getter and setter methods parse and rebuild the URL on each
// invocation; there is no attempt to cache the value and be more efficient
function URLDecompositionAttributes() {}
URLDecompositionAttributes.prototype = {
    constructor: URLDecompositionAttributes,

    get protocol() {
        var url = new URL(this.getInput());
        if (url.isAbsolute()) return url.scheme + ":";
        else return "";
    },

    get host() {
        var url = new URL(this.getInput());
        if (url.isAbsolute() && url.isAuthorityBased())
            return url.host + (url.port ? (":" + url.port) : "");
        else
            return "";
    },

    get hostname() {
        var url = new URL(this.getInput());
        if (url.isAbsolute() && url.isAuthorityBased())
            return url.host;
        else
            return "";
    },

    get port() {
        var url = new URL(this.getInput());
        if (url.isAbsolute() && url.isAuthorityBased() && url.port!==undefined)
            return url.port;
        else
            return "";
    },

    get pathname() {
        var url = new URL(this.getInput());
        if (url.isAbsolute() && url.isHierarchical())
            return url.path;
        else
            return "";
    },

    get search() {
        var url = new URL(this.getInput());
        if (url.isAbsolute() && url.isHierarchical() && url.query!==undefined)
            return "?" + url.query;
        else
            return "";
    },

    get hash() {
        var url = new URL(this.getInput());
        if (url.isAbsolute() && url.fragment != undefined)
            return "#" + url.fragment;
        else
            return "";
    },


    set protocol(v) {
        var output = this.getInput();
        var url = new URL(output);
        if (url.isAbsolute()) {
            v = v.replace(/:+$/, "");
            v = v.replace(/[^-+\.a-zA-z0-9]/g, URL.percentEncode);
            if (v.length > 0) {
                url.scheme = v;
                output = url.toString();
            }
        }
        this.setOutput(output);
    },

    set host(v) {
        var output = this.getInput();
        var url = new URL(output);
        if (url.isAbsolute() && url.isAuthorityBased()) {
            v = v.replace(/[^-+\._~!$&'()*,;:=a-zA-z0-9]/g, URL.percentEncode);
            if (v.length > 0) {
                url.host = v;
                delete url.port;
                output = url.toString();
            }
        }
        this.setOutput(output);
    },

    set hostname(v) {
        var output = this.getInput();
        var url = new URL(output);
        if (url.isAbsolute() && url.isAuthorityBased()) {
            v = v.replace(/^\/+/, "");
            v = v.replace(/[^-+\._~!$&'()*,;:=a-zA-z0-9]/g, URL.percentEncode);
            if (v.length > 0) {
                url.host = v;
                output = url.toString();
            }
        }
        this.setOutput(output);
    },

    set port(v) {
        var output = this.getInput();
        var url = new URL(output);
        if (url.isAbsolute() && url.isAuthorityBased()) {
            v = v.replace(/[^0-9].*$/, "");
            v = v.replace(/^0+/, "");
            if (v.length === 0) v = "0";
            if (parseInt(v, 10) <= 65535) {
                url.port = v;
                output = url.toString();
            }
        }
        this.setOutput(output);
    },

    set pathname(v) {
        var output = this.getInput();
        var url = new URL(output);
        if (url.isAbsolute() && url.isHierarchical()) {
            if (v.charAt(0) !== "/")
                v = "/" + v;
            v = v.replace(/[^-+\._~!$&'()*,;:=@\/a-zA-z0-9]/g,
                          URL.percentEncode);
            url.path = v;
            output = url.toString();
        }
        this.setOutput(output);
    },

    set search(v) {
        var output = this.getInput();
        var url = new URL(output);
        if (url.isAbsolute() && url.isHierarchical()) {
            if (v.charAt(0) !== "?") v = v.substring(1);
            v = v.replace(/[^-+\._~!$&'()*,;:=@\/?a-zA-z0-9]/g,
                          URL.percentEncode);
            url.query = v;
            output = url.toString();
        }
        this.setOutput(output);
    },

    set hash(v) {
        var output = this.getInput();
        var url = new URL(output);
        if (url.isAbsolute()) {
            if (v.charAt(0) !== "#") v = v.substring(1);
            v = v.replace(/[^-+\._~!$&'()*,;:=@\/?a-zA-z0-9]/g,
                          URL.percentEncode);
            url.fragment = v;
            output = url.toString();
        }
        this.setOutput(output);
    }
}



/************************************************************************
 *  src/impl/Location.js
 ************************************************************************/

//@line 1 "src/impl/Location.js"
function Location(window, href) {
    this._window = window;
    this._href = href;
}

Location.prototype = Object.create(URLDecompositionAttributes.prototype, {
    _idlName: constant("Location"),
    constructor: constant(Location),
    // The concrete methods that the superclass needs
    getInput: constant(function() { return this.href; }),
    setOutput: constant(function(v) { this.href = v; }),

    // Special behavior when href is set
    href: attribute(
        function() { return this._href; },
        function(v) { this.assign(v) }
    ),

    assign: constant(function(url) {
        // Resolve the new url against the current one
        // XXX:
        // This is not actually correct. It should be resolved against
        // the URL of the document of the script. For now, though, I only
        // support a single window and there is only one base url.
        // So this is good enough for now.
        var current = new URL(this._href);
        var newurl = current.resolve(url);
        var self = this; // for the XHR callback below

        // Save the new url
        this._href = newurl;

        // Start loading the new document!
        // XXX
        // This is just something hacked together.
        // The real algorithm is: http://www.whatwg.org/specs/web-apps/current-work/multipage/history.html#navigate
        var xhr = new XMLHttpRequest();

        xhr.onload = function() {
            var olddoc = self._window.document;
            var parser = new HTMLParser(newurl);
            var newdoc = unwrap(parser.document());
            newdoc.mutationHandler = olddoc.mutationHandler;

            // Get rid of any current content in the old doc
            // XXX
            // Should we have a special mutation event that means
            // discard the entire document because we're loading a new one?
            while(olddoc.hasChildNodes()) olddoc.removeChild(olddoc.firstChild);

            // Make the new document the current document in the window
            self._window.document = newdoc;
            newdoc.defaultView = self._window;

            // And parse the new file
            parser.parse(xhr.responseText, true);
        };

        xhr.open("GET", newurl);
        xhr.send();

    }),

    replace: constant(function(url) {
        // XXX
        // Since we aren't tracking history yet, replace is the same as assign
        this.assign(url);
    }),

    reload: constant(function() {
        // XXX:
        // Actually, the spec is a lot more complicated than this
        this.assign(this.href);
    }),

    // XXX: Does WebIDL allow the wrapper class to have its own toString
    // method? Or do I have to create a proxy just to fake out the string
    // conversion?
    // In FF, document.location.__proto__.hasOwnProperty("toString") is true
    toString: constant(function() {
        return this.href;
    }),
});



/************************************************************************
 *  src/impl/Window.js
 ************************************************************************/

//@line 1 "src/impl/Window.js"
// This is a simple constructor for a simple Window implementation
// We'll set things up (in src/main.js for now) so that it unwraps
// to the global object
function Window() {
    this.document = new impl.DOMImplementation().createHTMLDocument("");
    this.document._scripting_enabled = true;
    this.document.defaultView = this;
    this.location = new Location(this, "about:blank");

    // These numbers must match native code
    this.input = { 
        "resize": 0,  "mouse" : 1, "keyboard" : 2, 
        "spatialinput" : 3, "spatialmapping" : 4, "vsync": 5,
        "voice" : 6
    };

    this.callbackFromNative = function (type) {
        if (type === this.input.vsync) {
            var capturedCallback = holographic.drawCallback;
            holographic.drawCallback = null;
            if (capturedCallback) {
                capturedCallback();
            }
        } else if (type === this.input.resize) {
            var resizeEvent = this.document.createEvent("Event");
            resizeEvent.initEvent("resize", true, true);
            this.dispatchEvent(resizeEvent);
        } else if (type === this.input.spatialmapping) {
            if (this.onSpatialMapping) {
                this.onSpatialMapping(arguments[1]);
            }
        } else if (type === this.input.mouse) {
            var mouseEvent = holographic.canvas.dispatchMouseFromWindow(arguments[1], arguments[2], arguments[3], arguments[4], arguments[5]);
            holographic.canvas.ownerDocument.dispatchMouseFromWindow(mouseEvent);
        } else if (type === this.input.keyboard) {
            let keyEvent = holographic.canvas.dispatchKeyboardFromWindow(arguments[1], arguments[2]);
            this.dispatchEvent(keyEvent);
        } else if (type === this.input.spatialinput) {
            holographic.canvas.dispatchSpatialInputFromWindow(arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6]);
        } else if (type === this.input.voice) {
            let voiceEvent = holographic.canvas.dispatchVoiceFromWindow(arguments[1], arguments[2], arguments[3]);
            this.dispatchEvent(voiceEvent);
        }
    };

    holographic.nativeInterface.window.setCallback(this.callbackFromNative.bind(this));

    this.addEventListenerXXX = this.addEventListener;
    this.removeEventListenerXXX = this.removeEventListener;

    this.addEventListener = function (type, listener, capture) {
        if (type === "spatialmapping") {
            holographic.nativeInterface.window.requestSpatialMappingData(
                this._spatialMappingOptions.scanExtentMeters.x,
                this._spatialMappingOptions.scanExtentMeters.y,
                this._spatialMappingOptions.scanExtentMeters.z,
                this._spatialMappingOptions.trianglesPerCubicMeter);
            this.onSpatialMapping = listener;
        } else if (type === "voicecommand" || type === "keyup" || type === "keydown") {
            holographic.nativeInterface.input.addEventListener(type);
            this.addEventListenerXXX(type, listener, capture);
        } else {
            this.addEventListenerXXX(type, listener, capture);
        }
    };

    this.removeEventListener = function (type, listener, capture) {
        if (type === "spatialmapping") {
            delete this.onSpatialMapping;
        } else if (type === "voicecommand") {
            holographic.nativeInterface.input.removeEventListener(type);
            this.removeEventListenerXXX(type, listener, capture);
        } else {
            this.removeEventListenerXXX(type, listener, capture);
        }
    };

    this._spatialMappingOptions = { scanExtentMeters: { x: 5, y: 5, z: 3 }, trianglesPerCubicMeter: 100 };
    this._voiceCommands = [];
}

Window.prototype = O.create(impl.EventTarget.prototype, {
    _idlName: constant("Window"),

    history: constant({
        back: nyi,
        forward: nyi,
        go: nyi,
        _idlName: "History"
    }),

    navigator: constant({
        appName: "dom.js",
        appVersion: "0.1",
        platform: "JavaScript!",
        userAgent: "Servo",
        _idlName: "Navigator"
    }),

    // Self-referential properties
    window: attribute(function() { return this; }),
    self: attribute(function() { return this; }),
    frames: attribute(function() { return this; }),

    // Self-referential properties for a top-level window
    parent: attribute(function() { return this; }),
    top: attribute(function() { return this; }),

    // We don't support any other windows for now
    length: constant(0),           // no frames
    frameElement: constant(null),  // not part of a frame
    opener: constant(null),        // not opened by another window

    // The onload event handler.
    // XXX: need to support a bunch of other event types, too,
    // and have them interoperate with document.body.

    onload: attribute(
        function() {
            return this._getEventHandler("load");
        },
        function(v) {
            this._setEventHandler("load", v);
        }
    ),

    spatialMappingOptions: attribute(
        function () {
            return this._spatialMappingOptions;
        },
        function (v) {
            this._spatialMappingOptions = v;
        }
    ),

    voiceCommands: attribute(
        function () {
            return this._voiceCommands;
        },
        function (v) {
            holographic.nativeInterface.input.setVoiceCommands(v);
            this._voiceCommands = v;
        }
    ),
});



/************************************************************************
 *  src/main.js
 ************************************************************************/

//@line 1 "src/main.js"
/*
// The document object is the entry point to the entire DOM
defineLazyProperty(global, "document", function() {
    var doc = new impl.DOMImplementation().createHTMLDocument("");
    doc._scripting_enabled = true;
    return wrap(doc);
});
*/

// Create a window implementation object
var w = new Window();  // See src/impl/Window.js

// Arrange to have it wrap to the global object
// And have the global object unwrap to w
w._idl = global;
wmset(idlToImplMap, global, w);

// Now define window properties that do the right thing
// For other wrapper types this is automated by tools/idl2domjs
// but the window object is a special case.

[
    "window",
    "self",
    "frames",
    "parent",
    "top",
    "opener",
    "document",
    "history",
    "navigator"
].forEach(function(property) {
     Object.defineProperty(global, property, {
         get: function() {
             return wrap(unwrap(this)[property]);
         },
         enumerable: false,
         configurable: true, // XXX: check this
     });
 });

Object.defineProperty(global, "location", {
    get: function() {
        return wrap(unwrap(this).location);
    },
    set: function(v) {
        unwrap(this).location.href = v;
    },
    enumerable: false,
    configurable: true, // XXX: check this
});


Object.defineProperty(global, "onload", {
    get: function() {
        return unwrap(this).onload;
    },
    set: function(v) {
        unwrap(this).onload = toCallbackOrNull(v);
    },
    enumerable: true,
    configurable: true
});


global.addEventListener = function addEventListener(type, listener, capture) {
    unwrap(this).addEventListener(
        String(type),
        toCallbackOrNull(listener),
        OptionalBoolean(capture));
};

global.removeEventListener = function addEventListener(type, listener, capture){
    unwrap(this).removeEventListener(
        String(type),
        toCallbackOrNull(listener),
        OptionalBoolean(capture));
};

Object.defineProperty(global, "spatialMappingOptions", {
    get: function () {
        return unwrap(this).spatialMappingOptions;
    },
    set: function (v) {
        unwrap(this).spatialMappingOptions = v;
    },
    enumerable: false,
    configurable: true, // XXX: check this
});

Object.defineProperty(global, "voiceCommands", {
    get: function () {
        return unwrap(this).voiceCommands;
    },
    set: function (v) {
        unwrap(this).voiceCommands = v;
    },
    enumerable: false,
    configurable: true, // XXX: check this
});


// XXX
// This is a completely broken implementation put here just to see if we
// can get jQuery animations to work
//
global.getComputedStyle = function getComputedStyle(elt) {
    return elt.style;
};}(global));
}(this));
