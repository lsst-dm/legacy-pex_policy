
__all__ = []

from .policy import Policy

from lsst.utils import continueClass


@continueClass  # noqa: F811 (FIXME: remove for py 3.8+)
class Policy:  # noqa: F811
    def get(self, name):
        type = self.getValueType(name)
        if (type == self.UNDEF):
            return self.getInt(name)  # will raise an exception
            # raise NameNotFound("Policy parameter name not found: " + name)

        if (type == self.INT):
            return self.getInt(name)
        elif (type == self.DOUBLE):
            return self.getDouble(name)
        elif (type == self.BOOL):
            return self.getBool(name)
        elif (type == self.STRING):
            return self.getString(name)
        elif (type == self.POLICY):
            return self.getPolicy(name)
        elif (type == self.FILE):
            return self.getFile(name)

    def getArray(self, name):
        type = self.getValueType(name)
        if (type == self.UNDEF):
            return self.getIntArray(name)  # will raise an exception
            # raise NameNotFound("Policy parameter name not found: " + name)

        if (type == self.INT):
            return self.getIntArray(name)
        elif (type == self.DOUBLE):
            return self.getDoubleArray(name)
        elif (type == self.BOOL):
            return self.getBoolArray(name)
        elif (type == self.STRING):
            return self.getStringArray(name)
        elif (type == self.POLICY):
            return self.getPolicyArray(name)
        elif (type == self.FILE):
            return self.getFileArray(name)

    def set(self, name, value):
        if (value is None):
            raise RuntimeError("Attempt to set value of \"" + name +
                               "\" to None.  Values must be non-None.  Use remove() instead.")
        else:
            self._set(name, value)
