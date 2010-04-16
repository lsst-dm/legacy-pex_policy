/*
 * UrnPolicyFile
 */
#include "lsst/pex/policy/UrnPolicyFile.h"
#include "lsst/pex/exceptions.h"

#include <string>
#include <vector>

namespace lsst {
namespace pex {
namespace policy {

//@cond

using namespace std;

const string UrnPolicyFile::URN_PREFIX("urn:eupspkg:");
const string UrnPolicyFile::URN_PREFIX_ABBREV("@");

/**
 * Remove [@+][urn:eupspkg:].  That is,
 * "@urn:eupspkg:product:repos:file.paf", "urn:eupspkg:product:repos:file.paf",
 * "@@product:repos:file.paf", and "@product:repos:file.paf" all return the same
 * thing, "product:repos:file.paf".
 *
 * Note: pass urn by value because we're going to modify it anyway.
 */
string stripPrefixes(string urn) {
    // strip @'s
    while (urn.length() > 0 && urn.find(UrnPolicyFile::URN_PREFIX_ABBREV) == 0)
	urn = urn.substr(UrnPolicyFile::URN_PREFIX_ABBREV.length());
    // strip urn:eupspkg:
    string lowered(urn);
    transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
    if (lowered.find(UrnPolicyFile::URN_PREFIX) == 0)
	urn = urn.substr(UrnPolicyFile::URN_PREFIX.length());
    return urn;
}

/**
 * Remove prefixes, split at colons, and check that the number of terms in the
 * address is valid.  For example, "@urn:eupspkg:product:repos:file",
 * "@@product:repos:file" and "product:repos:file" all become [product, repos,
 * file].
 */
void splitAndValidate(const string& urn, vector<string>& a) {
    // strip prefixes
    string stripped = stripPrefixes(urn);

    // split
    while (true) {
	size_t i = stripped.find(":");
	if (i == string::npos) {
	    if (stripped.length() > 0) a.push_back(stripped);
	    break;
	}
	else {
	    a.push_back(stripped.substr(0, i));
	    stripped = stripped.substr(i + 1);
	}
    }

    // validate
    // - min size is 2 -- product:file
    // - max size is 3 -- product:repos:file
    if (a.size() < 2 || a.size() > 3)
	throw LSST_EXCEPT
	    (BadNameError,
	     "Wrong number of terms in policy file urn \"" + urn + "\".  "
	     + "The expected form is "
	     + "@urn:eupspkg:<product>:[<repository>:]<file> or "
	     + "@@<product>:[<repository>:]<file>.  "
	     + "Is there a typo in the urn?");
}

/**
 * Extract the product name from a URN.  For example,
 *  - @urn:eupspkg:PRODUCT:repos:path/to/file.paf
 *  - @@PRODUCT:repos:path/to/file.paf
 *  - @PRODUCT:path/to/file.paf
 */
string UrnPolicyFile::productNameFromUrn(const string& urn) {
    vector<string> split;
    splitAndValidate(urn, split);
    return split[0];
}

/**
 * Extract the local file path from a URN.  For example,
 *  - @urn:eupspkg:product:repos:PATH/TO/FILE.PAF
 *  - @@product:repos:PATH/TO/FILE.PAF
 *  - @product:PATH/TO/FILE.PAF
 */
string UrnPolicyFile::filePathFromUrn(const string& urn) {
    vector<string> split;
    splitAndValidate(urn, split);
    return split.back();
}

/**
 * Extract the repository name from a URN, or "" if none.  For example,
 *  - @urn:eupspkg:product:REPOS:path/to/file.paf
 *  - @@product:REPOS:path/to/file.paf
 *  - @product:path/to/file.paf -- no repository, so ""
 */
string UrnPolicyFile::reposFromUrn(const string& urn) {
    vector<string> split;
    splitAndValidate(urn, split);
    if (split.size() == 3) return split[1];
    else return "";
}

/**
 * Does `s` look like a URN?  That is, does it start with URN_PREFIX or
 * URN_PREFIX_ABBREV and have at least one colon after the prefix?
 */
bool UrnPolicyFile::looksLikeUrn(const string& s) {
    const string& stripped = stripPrefixes(s);
    return s.length() != stripped.length() && s.find(":") >= 0;
}

//@endcond

} // namespace policy
} // namespace pex
} // namespace lsst
