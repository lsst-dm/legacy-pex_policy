/**
 * @file Policy_3.cc
 *
 * This test tests format detection and format-agnostic loading of policy 
 * data.  
 */
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <list>
#include "lsst/pex/policy/paf/PAFParser.h"
#include "lsst/pex/policy.h"
#include "lsst/pex/policy/PolicyFile.h"

using namespace std;
using lsst::pex::policy::Policy;
using lsst::pex::policy::PolicyFile;
using lsst::pex::policy::TypeError;
using lsst::pex::policy::NameNotFound;

#define Assert(b, m) tattle(b, m, __LINE__)

void tattle(bool mustBeTrue, const string& failureMsg, int line) {
    if (! mustBeTrue) {
        ostringstream msg;
        msg << __FILE__ << ':' << line << ":\n" << failureMsg << ends;
        throw runtime_error(msg.str());
    }
}

int main(int argc, char** argv) {

    Policy p;
    string files[] = { 
        string("examples/EventTransmitter_policy.paf"),
        string("examples/pipeline_policy.paf") 
    };
    int nfiles = 2;

    if (argc > 1) {
        files[0] = argv[1];
        nfiles = 1;
    }

    for(int i=0; i < nfiles; i++) {
        PolicyFile pfile(files[i]);
        cout << "Contents of " << pfile.getFormatName() << " file, " 
             << files[i] << endl;

        Policy pol(pfile);
        pol.loadPolicyFiles("examples", false);
        cout << pol << endl;

        if (i==0) {
            Assert(pol.getBool("standalone"), "wrong value: standalone");
            Assert(pol.getDouble("threshold") == 4.5, /* parasoft-suppress LsstDm-5-12 "unittest" */
                   "wrong value: threshold");
            Assert(pol.getInt("offsets") == 313, "wrong value: offsets");
            Assert(pol.valueCount("offsets") == 8,
                   "wrong # of values: offsets");
            Assert(pol.getString("receiver.logVerbosity") == "debug",
                   "wrong value: receiver.logVerbosity");
            Assert(pol.getString("transmitter.logVerbosity") == "debug",
                   "wrong value: transmitter.logVerbosity");
            Assert(pol.getString("transmitter.serializationFormat") == "deluxe",
                   "wrong value: transmitter.serializationFormat");
            Assert(pol.getString("polish") == "fancy","wrong value: polish");
        }
    }

}
