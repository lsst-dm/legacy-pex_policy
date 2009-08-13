/*
 * Dictionary
 */
#include "lsst/pex/policy/Dictionary.h"
#include "lsst/pex/policy/PolicyFile.h"
// #include "lsst/pex/utils/Trace.h"

#include <stdexcept>
#include <iostream> // TODO: remove after debugging
#include <memory>
#include <string>
#include <set>
#include <boost/scoped_ptr.hpp>
// #include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

namespace lsst {
namespace pex {
namespace policy {

//@cond

using boost::regex;
using boost::sregex_token_iterator;
using boost::scoped_ptr;
using namespace std;
using namespace boost;

const string ValidationError::EMPTY;

ValidationError::MsgLookup ValidationError::_errmsgs;

void ValidationError::_loadMessages() {
    _errmsgs[OK] = EMPTY;
    _errmsgs[WRONG_TYPE] = "value has the incorrect type";
    _errmsgs[MISSING_REQUIRED] = "no value available for required parameter";
    _errmsgs[NOT_AN_ARRAY] = "value is not an array as required";
    _errmsgs[ARRAY_TOO_SHORT] = "insufficient number of array values";
    _errmsgs[TOO_FEW_VALUES] = "not enough values for parameter";
    _errmsgs[TOO_MANY_VALUES] = "too many values provided for parameter";
    _errmsgs[WRONG_OCCURRENCE_COUNT]="incorrect number of values for parameter";
    _errmsgs[VALUE_DISALLOWED] = "value is not among defined set";
    _errmsgs[VALUE_OUT_OF_RANGE] = "value is out of range";
    _errmsgs[BAD_VALUE] = "illegal value";
    _errmsgs[UNKNOWN_NAME] = "parameter name is unknown";
    _errmsgs[BAD_DEFINITION] = "malformed definition";
    _errmsgs[NOT_LOADED] = "file not loaded"
	" -- call Policy.loadPolicyFiles() before validating";
    _errmsgs[UNKNOWN_ERROR] = "unknown error";
}

vector<string> ValidationError::getParamNames() const {
    std::vector<std::string> result;
    ParamLookup::const_iterator i;
    for (i = _errors.begin(); i != _errors.end(); ++i)
        result.push_back(i->first);
    return result;
}

string ValidationError::describe(string prefix) const {
    ostringstream os;
    std::list<std::string> names;
    paramNames(names);
    for (std::list<string>::const_iterator i = names.begin(); i != names.end(); ++i) {
        int errs = getErrors(*i);
        os << prefix << *i << ": " << getErrorMessageFor((ErrorType)errs) << endl;
/*
        for (map<int,string>::const_iterator j = _errmsgs.begin(); j != _errmsgs.end(); ++j)
            if ((errs & j->first) == j->first)
                os << "  * " << j->second << endl;
*/
    }
    return os.str();
}

char const* ValidationError::what(void) const throw() {
    // static to avoid memory issue -- but a concurrency problem?
    // copied from pex/exceptions/src/Exception.cc
    static std::string buffer;
    ostringstream os;
    os << "Validation error: ";
    if (getParamCount() == 0)
	os << "no errors" << "\n";
    else {
	os << "\n" << describe("  * ");
    }
    buffer = os.str();
    return buffer.c_str();
}

ValidationError::~ValidationError() throw() { }

///////////////////////////////////////////////////////////
//  Definition
///////////////////////////////////////////////////////////

Definition::~Definition() { }

Policy::ValueType Definition::_determineType() const {
    if (_policy->isString("type")) {
        const string& type = _policy->getString("type");
	Policy::ValueType result;
	try {
	    result = Policy::getTypeByName(type);
	} catch(BadNameError&) {
	    throw LSST_EXCEPT
		(DictionaryError, string("Unknown type: \"") + type + "\".");
	}
	if (result == Policy::FILE)
	    throw LSST_EXCEPT(DictionaryError, string("Illegal type: \"") + type
			      + "\"; use \"" + Policy::typeName[Policy::POLICY]
			      + "\" instead.");
	else return result;
    }
    else if (_policy->exists("type")) {
	throw LSST_EXCEPT
	    (DictionaryError, string("Expected string for \"type\"; found ") 
	     + _policy->getTypeName("type") + " instead.");
    }

    else return Policy::UNDEF;
}
    
/*
 * the default value into the given policy
 * @param policy   the policy object update
 * @param withName the name to look for the value under.  This must be 
 *                    a non-hierarchical name.
 */
void Definition::setDefaultIn(Policy& policy, const string& withName) const {
    if (! _policy->exists("default")) return;
    Policy::ValueType type = getType();
    if (type == Policy::UNDEF) 
        type = _policy->getValueType("default");

    switch (type) {
    case Policy::BOOL: 
    {
        const Policy::BoolArray& defs = _policy->getBoolArray("default");
        Policy::BoolArray::const_iterator it;
        for(it=defs.begin(); it != defs.end(); ++it) {
            if (it == defs.begin()) 
                // erase previous values
                policy.set(withName, *it);
            else 
                policy.add(withName, *it);
        }
        break;
    }

    case Policy::INT:
    {
        const Policy::IntArray& defs = 
            _policy->getIntArray("default");
        Policy::IntArray::const_iterator it;
        for(it=defs.begin(); it != defs.end(); ++it) {
            if (it == defs.begin()) 
                // erase previous values
                policy.set(withName, *it);
            else 
                policy.add(withName, *it);
        }
        break;
    }

    case Policy::DOUBLE:
    {
        const Policy::DoubleArray& defs = 
            _policy->getDoubleArray("default");
        Policy::DoubleArray::const_iterator it;
        for(it=defs.begin(); it != defs.end(); ++it) {
            if (it == defs.begin()) 
                // erase previous values
                policy.set(withName, *it);
            else 
                policy.add(withName, *it);
        }
        break;
    }

    case Policy::STRING:
    {
        const Policy::StringArray defs = 
            _policy->getStringArray("default");
        Policy::StringArray::const_iterator it;
        for(it=defs.begin(); it != defs.end(); ++it) {
            if (it == defs.begin()) 
                // erase previous values
                policy.set(withName, *it);
            else 
                policy.add(withName, *it);
        }
        break;
    }

    case Policy::POLICY:
    {
        const Policy::PolicyPtrArray& defs = 
            _policy->getPolicyArray("default");
        Policy::PolicyPtrArray::const_iterator it;
        for(it=defs.begin(); it != defs.end(); ++it) {
            if (it == defs.begin()) 
                // erase previous values
                policy.set(withName, *it);
            else 
                policy.add(withName, *it);
        }
        break;
    }

    default:
        throw LSST_EXCEPT(pexExcept::LogicErrorException,
                          string("Programmer Error: Unknown type: ") + Policy::typeName[getType()]);
    }
}

/*
 * confirm that a Policy parameter conforms to this definition
 * @param policy   the policy object to inspect
 * @param name     the name to look for the value under.  If not given
 *                  the name set in this definition will be used.
 * @exception ValidationError   if the value does not conform.  The message
 *                 should explain why.
 */
void Definition::validate(const Policy& policy, const string& name,
                          ValidationError *errs) const 
{ 
    ValidationError ve(LSST_EXCEPT_HERE);
    ValidationError *use = &ve;
    if (errs != 0) use = errs;

    if (! policy.exists(name)) {
        try {
            if (_policy->getInt("minOccurs") > 0)
                use->addError(name, ValidationError::MISSING_REQUIRED);
        }
        catch (NameNotFound&) { }
        return;
    }

    // What type is actually present in the policy?
    Policy::ValueType type = policy.getValueType(name);

    switch (type) {
    case Policy::BOOL: 
        validate<bool>(name, policy, use);
        break;

    case Policy::INT:
        validate<int>(name, policy, use);
        break;

    case Policy::DOUBLE:
        validate<double>(name, policy, use);
        break;

    case Policy::STRING:
        validate<string>(name, policy, use);
        break;

    case Policy::POLICY:
        validate<Policy::Ptr>(name, policy, use);
        break;

    case Policy::FILE:
        use->addError(name, ValidationError::NOT_LOADED);
        break;

    default:
        throw LSST_EXCEPT(pexExcept::LogicErrorException,
                          string("Unknown type: ") + policy.getTypeName(name));
    }

    if (errs == 0 && ve.getParamCount() > 0) throw ve;
}

/**
 * Validate the number of values for a field. Used internally by the
 * validate() functions. 
 * @param name   the name of the parameter being checked
 * @param count  the number of values this name actually has
 * @param errs   report validation errors here
 */
void Definition::validateCount(const string& name, int count,
                               ValidationError *errs) const {
    int max = getMaxOccurs(); // -1 means no limit / undefined
    if (max >= 0 && count > max) 
        errs->addError(name, ValidationError::TOO_MANY_VALUES);
    if (count < getMinOccurs()) {
        if (count == 0) 
            errs->addError(name, ValidationError::MISSING_REQUIRED);
        else if (count == 1) 
            errs->addError(name, ValidationError::NOT_AN_ARRAY);
        else 
            errs->addError(name, ValidationError::ARRAY_TOO_SHORT);
    }
}

template <class T>
void Definition::validate(const string& name, const Policy& policy,
			  ValidationError *errs) const
{
    validate(name, policy.getValueArray<T>(name), errs);
}

template <class T>
void Definition::validate(const string& name, const vector<T>& value,
			  ValidationError *errs) const
{
    ValidationError ve(LSST_EXCEPT_HERE);
    ValidationError *use = &ve;
    if (errs != 0) use = errs;

    validateCount(name, value.size(), use);

    for (typename vector<T>::const_iterator i = value.begin(); i != value.end(); ++i)
	validate<T>(name, *i, -1, use);

    if (errs == 0 && ve.getParamCount() > 0) throw ve;
}

/**
 * Stubs for validation template functions.  Always return true, which always
 * failes validation tests.  In other words, any values of min or max for these
 * types will cause a failure.
 */
bool operator<(const Policy& a, const Policy& b) { return true; }
bool operator<(const Policy::ConstPtr& a, const Policy::ConstPtr& b) { return true; }
bool operator<(const Policy::FilePtr& a, const Policy::FilePtr& b) { return true; }

template <class T>
void Definition::validate(const string& name, const T& value,
			  int curcount, ValidationError *errs) const
{
    ValidationError ve(LSST_EXCEPT_HERE);
    ValidationError *use = &ve;
    if (errs != 0) use = errs;
    
    // check if we're going to get too many
    if (curcount >= 0) {
        int maxOccurs = getMaxOccurs();
        if (maxOccurs >= 0 && curcount + 1 > maxOccurs) 
            use->addError(name, ValidationError::TOO_MANY_VALUES);
    }

    if (getType() != Policy::UNDEF && getType() != Policy::getValueType<T>()) {
        use->addError(name, ValidationError::WRONG_TYPE);
    }
    else if (_policy->isPolicy("allowed")) {
        Policy::PolicyPtrArray allowed = _policy->getPolicyArray("allowed");

        T min, max;
        bool minFound = false, maxFound = false;
        set<T> allvals;
        for (Policy::PolicyPtrArray::const_iterator it = allowed.begin();
             it != allowed.end(); ++it)
        {
            Policy::Ptr a = *it;
            if (a->exists("min")) {
                if (minFound) {
		    // TODO: catch this in Dictionary::check()
                    throw LSST_EXCEPT
                        (DictionaryError, string("Min value for ") + name
			 + " (" + lexical_cast<string>(min) 
			 + ") already specified; additional value not allowed.");
		}
		try {
		    min = a->getValue<T>("min");
		} catch(TypeError& e) {
		    throw LSST_EXCEPT
			(DictionaryError, string("Wrong type for ") + name 
			 + " min value: expected " + getTypeName() + ", found \"" 
			 + lexical_cast<string>(max) + "\".");
		} catch(...) {
		    throw;
		}
                minFound = true; // after min assign, in case of exceptions
            }
            if (a->exists("max")) {
                if (maxFound)
                    throw LSST_EXCEPT
                        (DictionaryError, string("Max value for ") + name
			 + " (" + lexical_cast<string>(max) 
			 + ") already specified; additional value not allowed.");
		try {
		    max = a->getValue<T>("max");
		} catch(TypeError& e) {
		    throw LSST_EXCEPT
			(DictionaryError, string("Wrong type for ") + name 
			 + " max value: expected " + getTypeName() + ", found \"" 
			 + lexical_cast<string>(max) + "\".");
		}
                maxFound = true; // after max assign, in case of exceptions
            }
            if (a->exists("value")) allvals.insert(a->getValue<T>("value"));
        }
        cout << "** min = " << (minFound ? lexical_cast<string>(min) : "none") 
             << "; max = " << (maxFound ? lexical_cast<string>(max) : "none" ) << endl;

        if ((minFound && value < min) || (maxFound && max < value))
            use->addError(name, ValidationError::VALUE_OUT_OF_RANGE);

        if (allvals.size() > 0 && allvals.count(value) == 0) {
            cout << "## value " << value << " not allowed for " << name << endl;
            use->addError(name, ValidationError::VALUE_DISALLOWED);
        }
    }
    if (errs == 0 && ve.getParamCount() > 0) throw ve;
}

/*
 * confirm that a Policy parameter name-value combination is consistent 
 * with this dictionary.  This does not check occurrence compliance
 * @param name     the name of the parameter being checked
 * @param value    the value of the parameter to check.
 * @exception ValidationError   if the value does not conform.  The message
 *                 should explain why.
 */
void Definition::validate(const string& name, bool value, int curcount,
                          ValidationError *errs) const 
{ 
    validate<bool>(name, value, curcount, errs);
}

void Definition::validate(const string& name, int value, int curcount,
                          ValidationError *errs) const 
{ 
    validate<int>(name, value, curcount, errs);
}

void Definition::validate(const string& name, double value, int curcount,
                          ValidationError *errs) const 
{ 
    validate<double>(name, value, curcount, errs);
}

void Definition::validate(const string& name, string value, int curcount,
                          ValidationError *errs) const 
{ 
    validate<string>(name, value, curcount, errs);
}

void Definition::validate(const string& name, const Policy& value, 
                          int curcount, ValidationError *errs) const 
{ 
    validate<Policy>(name, value, curcount, errs);
}

/*
 * confirm that a Policy parameter name-array value combination is 
 * consistent with this dictionary.  Unlike the scalar version, 
 * this does check occurrence compliance.  
 * @param name     the name of the parameter being checked
 * @param value    the value of the parameter to check.
 * @exception ValidationError   if the value does not conform.  The message
 *                 should explain why.
 */
void Definition::validate(const string& name, const Policy::BoolArray& value, 
                          ValidationError *errs) const 
{ 
    validate<bool>(name, value, errs);
}

void Definition::validate(const string& name, const Policy::IntArray& value, 
                          ValidationError *errs) const 
{ 
    validate<int>(name, value, errs);
}

void Definition::validate(const string& name, const Policy::DoubleArray& value,
                          ValidationError *errs) const 
{ 
    validate<double>(name, value, errs);
}
void Definition::validate(const string& name, const Policy::StringArray& value, 
                          ValidationError *errs) const 
{ 
    validate<string>(name, value, errs);
}

void Definition::validate(const string& name, const Policy::ConstPolicyPtrArray& value, 
                          ValidationError *errs) const 
{ 
    validate<Policy::ConstPtr>(name, value, errs);
}

///////////////////////////////////////////////////////////
//  Dictionary
///////////////////////////////////////////////////////////

const regex Dictionary::FIELDSEP_RE("\\.");

/*
 * load a dictionary from a file
 */
Dictionary::Dictionary(const char *filePath) : Policy(filePath) { 
    if (!exists("definitions"))
        throw LSST_EXCEPT(pexExcept::RuntimeErrorException, string(filePath) + ": does not contain a dictionary");
}
Dictionary::Dictionary(const string& filePath) : Policy(filePath) { 
    if (!exists("definitions"))
        throw LSST_EXCEPT(pexExcept::RuntimeErrorException, string(filePath) + ": does not contain a dictionary");
}
Dictionary::Dictionary(const PolicyFile& filePath) : Policy(filePath) { 
    if (!exists("definitions"))
        throw LSST_EXCEPT(pexExcept::RuntimeErrorException, filePath.getPath() + ": does not contain a dictionary");
}

/*
 * return a definition for the named parameter.  The caller is responsible
 * for deleting the returned object.  This is slightly more efficient than
 * getDef().
 * @param name    the hierarchical name for the parameter
 */
Definition* Dictionary::makeDef(const string& name) const {
    Policy *p = const_cast<Dictionary*>(this);
    Policy::Ptr sp;

    // split the name
    sregex_token_iterator it = make_regex_token_iterator(name,FIELDSEP_RE, -1);
    sregex_token_iterator end;
    string find;
    while (it != end) {
        find = *it;
        if (! p->isPolicy("definitions"))
            throw LSST_EXCEPT(DictionaryError, "Definition for " + find + " not found.");
        sp = p->getPolicy("definitions");
        if (! sp->isPolicy(find)) throw LSST_EXCEPT(NameNotFound, find);
        sp = sp->getPolicy(find);
        p = sp.get();
        if (++it != end) {
            if (! sp->isPolicy("dictionary"))
                throw LSST_EXCEPT(DictionaryError, find + ".dictionary not found.");
            sp = sp->getPolicy("dictionary");
            p = sp.get();
        }
    }
    // TODO: if no definition found, look for childDefinition
    return new Definition(name, sp);
}

void Dictionary::loadPolicyFiles(const fs::path& repository, bool strict) {
    // find the "dictionaryFile" parameters
    std::list<std::string> params;
    paramNames(params, false);
    std::list<std::string>::iterator ni;
    for (ni=params.begin(); ni != params.end(); ++ni) { 
        std::string endswith(".dictionaryFile");
        size_t p = ni->rfind(endswith);
        if (p == ni->length()-endswith.length()) {
            std::string parent = ni->substr(0, p);
            Policy::Ptr defin = getPolicy(parent);
            PolicyFile subd;

            // these will get dereferenced with the call to super method
            if (isFile(*ni)) 
                defin->set("dictionary", getFile(*ni));
            else
                defin->set("dictionary", 
                           Policy::FilePtr(new PolicyFile(getString(*ni))));
        }
    }

    Policy::loadPolicyFiles(repository, strict);
}

/**
 * Check this Dictionary's internal integrity.  Load up all definitions and
 * sanity-check them.
 */
void Dictionary::check() const {
    PolicyPtrArray defs = getValueArray<Policy::Ptr>("definitions");
    if (defs.size() == 0)
	throw LSST_EXCEPT(DictionaryError, "no \"definitions\" section found");
    if (defs.size() > 1)
	throw LSST_EXCEPT
	    (DictionaryError, string("expected a single \"definitions\" section; "
				     "found ") + lexical_cast<string>(defs.size()));
    Policy::StringArray names = defs[0]->names(false);
    for (Policy::StringArray::const_iterator i = names.begin(); i != names.end(); ++i) {
	cout << " %%% checking \"" << *i << "\" %%%" << endl;
	scoped_ptr<Definition> def(makeDef(*i));
    }
}

/*
 * validate a Policy against this Dictionary
 */
void Dictionary::validate(const Policy& pol, ValidationError *errs) const { 
    ValidationError ve(LSST_EXCEPT_HERE);
    ValidationError *use = &ve;
    if (errs != 0) use = errs;
    Policy::StringArray params = pol.names(true);
    Policy::StringArray::iterator ni;
    for(ni = params.begin(); ni != params.end(); ++ni) {
        try {
            scoped_ptr<Definition> def(makeDef(*ni));
            def->validate(pol, *ni, use);
        }
        catch (NameNotFound& e) {
            use->addError(*ni,ValidationError::UNKNOWN_NAME);
        }
        catch (TypeError& e) {
            throw LSST_EXCEPT(pexExcept::LogicErrorException, string("Programmer Error: Param's type morphed: ") + e.what());
        }
        cout << "-- errors after " << *ni << ": " << use->getParamCount()
             << " / " << use->getErrors(*ni) << endl;
    }
    // TODO: handle missing elements (minOccurs >= 1)
    // TODO: handle NameNotFound as a validation error -- add to errs -- rather than simply throwing an exception

    cout << "** errors at end: " << use->getParamCount() << endl;
    if (errs == 0 && ve.getParamCount() > 0) throw ve;
}

//@endcond

} // namespace policy
} // namespace pex
} // namespace lsst
