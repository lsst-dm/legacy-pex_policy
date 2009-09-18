import optparse

from lsst.pex.policy import Policy, PolicyFile, Dictionary, ValidationError
from lsst.pex.exceptions import LsstCppException

usage = """usage: %prog <options> [policy] [dictionary]"""

desc = """
Use a dictionary file to validate a policy file.
"""

# see http://docs.python.org/library/optparse.html
parser = optparse.OptionParser(usage=usage, description=desc)
parser.add_option("-d", "--dictionary", dest="dictionary", metavar="FILE",
                  help="the dictionary to validate a policy against")
parser.add_option("-p", "--policy", dest="policy", metavar="FILE",
                  help="the policy to validate")
parser.add_option("-l", "--load-policy-references", dest="load_policy", metavar="FILE",
                  help="directory from which to load policy file references")
parser.add_option("-L", "--load-dict-references", dest="load_policy", metavar="FILE",
                  help="directory from which to load dictionary file references")
parser.add_option("-D", "--load-defaults", dest="defaults", metavar="FILE",
                  help="load defaults from FILE, which can be a policy or dictionary")
parser.add_option("-v", "--verbose", dest="verbosity", action="store_const", const=1,
                  help="print extra messages")
(options, args) = parser.parse_args()

def tryThis(callableObj, explain, *args, **kwargs):
    try:
        if verbosity > 0: print explain
        result = callableObj(*args, **kwargs)
        return result
    except LsstCppException, e:
        print "error", explain + ":"
        print e.args[0].what()
        sys.exit(2)

if __name__ == "__main__":
    policyFile = options.policy
    dictFile = options.dictionary
    if (policyFile == None):
        if len(args) < 1:
            parser.error("no policy specified")
        policyFile = args[0]
        del(args[0])
    if (dictFile == None):
        if len(args) < 1:
            parser.error("no dictionary specified")
        dictFile = args[0]
        del(args[0])

    defaultsFile = options.defaults
    print "--- defaults:", defaultsFile

    if len(args) != 0:
        parser.error("incorrect number of arguments")
    # print "args =", args, len(args)
    # print "options =", options
    policy = None
    verbosity = 0
    if (options.verbosity != None): verbosity = options.verbosity

    # 1. load policy
    policy = tryThis(Policy, "reading policy file \"" + policyFile + "\"", policyFile)

    # 2. merge defaults into policy (if specified)
    defaults = None
    if (defaultsFile != None):
        defaults = tryThis(Policy, "reading defaults from \"" + defaultsFile + "\"",
                           defaultsFile)
        tryThis(policy.mergeDefaults, "merging defaults into policy", defaults)

    # 3. load dictionary
    dictionary = tryThis(Dictionary, "reading dictionary file \"" + dictFile + "\"",
                         dictFile)
    
    # 4. validate
    tryThis(dictionary.validate, "validating " + policyFile + " against " + dictFile,
            policy)

    if verbosity > 0: print
    print "Validation succeeded:", policyFile, "is a valid instance of", dictFile
