from . import _policy
from . import paf

import lsst.bputils

_policy.Policy._set = _policy.Policy.set
_policy.Policy._add = _policy.Policy.add

lsst.bputils.rescope(_policy, globals(), ignore=("Policy", "ValidationError", "PAFWriter"))

@lsst.bputils.extend(_policy.Policy)
class Policy:
    
    def get(p, name):
        type = p.getValueType(name);
        if (type == p.UNDEF):
            return p.getInt(name) # will raise an exception
            # raise NameNotFound("Policy parameter name not found: " + name)

        if (type == p.INT):
            return p.getInt(name)
        elif (type == p.DOUBLE):
            return p.getDouble(name)
        elif (type == p.BOOL):
            return p.getBool(name)
        elif (type == p.STRING):
            return p.getString(name)
        elif (type == p.POLICY):
            return p.getPolicy(name)
        elif (type == p.FILE):
            return p.getFile(name)

    def getArray(p, name):
        type = p.getValueType(name);
        if (type == p.UNDEF):
            return p.getIntArray(name) # will raise an exception
            # raise NameNotFound("Policy parameter name not found: " + name)

        if (type == p.INT):
            return p.getIntArray(name)
        elif (type == p.DOUBLE):
            return p.getDoubleArray(name)
        elif (type == p.BOOL):
            return p.getBoolArray(name)
        elif (type == p.STRING):
            return p.getStringArray(name)
        elif (type == p.POLICY):
            return p.getPolicyArray(name)
        elif (type == p.FILE):
            return p.getFileArray(name)

    def set(p, name, value):
        if isinstance(value, bool):
            p._setBool(name, value)
        elif (value == None):
            raise RuntimeError("Attempt to set value of \"" + name 
                               + "\" to None.  Values must be non-None.  Use remove() instead.")
    #        raise lsst.pex.exceptions.InvalidParameterException("Value of " + name + " cannot be None.")
        else:
            p._set(name, value)

    def add(p, name, value):
        if isinstance(value, bool):
            p._addBool(name, value)
        else:
            p._add(name, value)

@lsst.bputils.extend(_policy.ValidationError)
class ValidationError:

    def __init__(self, *args, **kwds):
        return lsst.pex.exceptions.LogicErrorException.__init__(
            self, _policy.LsstCppValidationError(*args, **kwds)
            )

    ErrorType = _policy.LsstCppValidationError.ErrorType

for name, value in ValidationError.ErrorType.names.iteritems():
    setattr(ValidationError, name, value)
