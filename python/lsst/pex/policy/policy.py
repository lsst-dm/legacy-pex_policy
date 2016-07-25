from _policy import *

Policy.__str__ = Policy.toString

def _Policy_get(p, name):
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

def _Policy_getArray(p, name):
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

Policy.get = _Policy_get
Policy.getArray = _Policy_getArray

_Policy_wrap_set = Policy.set
def _Policy_set(p, name, value):
    if (value == None):
        raise RuntimeError("Attempt to set value of \"" + name + "\" to None.  Values must be non-None.  Use remove() instead.")
    else:
        _Policy_wrap_set(p, name, value)
Policy.set = _Policy_set

