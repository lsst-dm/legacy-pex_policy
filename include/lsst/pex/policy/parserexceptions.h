// -*- lsst-c++ -*-
/*
 * @ingroup pex
 *
 * @brief exceptions characterizing errors in the use of Policies
 * 
 * @author Ray Plante
 * 
 */

#ifndef LSST_PEX_POLICY_PARSER_EXCEPTIONS_H
#define LSST_PEX_POLICY_PARSER_EXCEPTIONS_H

#include <stdexcept>
#include <sstream>

namespace lsst {
namespace pex {
namespace policy {

/**
 * an parent exception for errors that occur during the parsing of policy 
 * files.  
 */
class ParserError : public pexExcept::RuntimeErrorException {
public:

    /**
     * Create an exception the exception with a default message.
     */
    ParserError(LSST_EARGS_TYPED) 
        : pexExcept::RuntimeErrorException(LSST_EARGS_UNTYPED, 
                                       "Unspecified parsing error encountered") 
    { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     */
    ParserError(LSST_EARGS_TYPED, const std::string& msg) 
        : pexExcept::RuntimeErrorException(LSST_EARGS_UNTYPED, msg) 
    { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     * @param lineno  a line number in the file (or stream) being parsed 
     *                  where the problem occurred.  The first line of the 
     *                  file is typically line 1.  
     */
    ParserError(LSST_EARGS_TYPED, const std::string& msg, int lineno) 
        : pexExcept::RuntimeErrorException(LSST_EARGS_UNTYPED, 
                                           makeLocatedMessage(msg,lineno)) 
    { }

    static std::string makeLocatedMessage(const std::string& msg, int lineno) {
        std::ostringstream out;
        out << "Policy Parsing Error:" << lineno << ": " << msg << ends;
        return out.str();
    }
};

/**
 * an exception indicated that the stream being parsed ended prematurely.
 */
class EOFError : public ParserError {
public:

    /**
     * Create an exception the exception with a default message.
     */
    EOFError(LSST_EARGS_TYPED) 
        : ParserError(LSST_EARGS_UNTYPED, 
                      "Unexpected end of Policy data stream") 
    { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     */
    EOFError(LSST_EARGS_TYPED, const std::string& msg) 
        : ParserError(LSST_EARGS_UNTYPED, msg) { }

    /**
     * Create an exception the exception with a default message.
     * @param lineno  a line number in the file (or stream) being parsed 
     *                  where the problem occurred.  The first line of the 
     *                  file is typically line 1.  
     */
    EOFError(LSST_EARGS_TYPED, int lineno) 
        : ParserError(LSST_EARGS_UNTYPED, 
                      "Unexpected end of Policy data stream", lineno) 
    { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     * @param lineno  a line number in the file (or stream) being parsed 
     *                  where the problem occurred.  The first line of the 
     *                  file is typically line 1.  
     */
    EOFError(LSST_EARGS_TYPED, const std::string& msg, int lineno) 
        : ParserError(LSST_EARGS_UNTYPED, msg, lineno) { }
};

/**
 * an exception thrown because a general syntax error was encountered.
 */
class SyntaxError : public ParserError {
public:

    /**
     * Create an exception the exception with a default message.
     */
    SyntaxError(LSST_EARGS_TYPED) 
        : ParserError(LSST_EARGS_UNTYPED, "Unknonwn syntax error") { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     */
    SyntaxError(LSST_EARGS_TYPED, const std::string& msg) 
        : ParserError(LSST_EARGS_UNTYPED, msg) { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     * @param lineno  a line number in the file (or stream) being parsed 
     *                  where the problem occurred.  The first line of the 
     *                  file is typically line 1.  
     */
    SyntaxError(LSST_EARGS_TYPED, const std::string& msg, int lineno) 
        : ParserError(LSST_EARGS_UNTYPED, msg, lineno) { }

};

/**
 * an exception thrown because a syntax error specific to the format
 * being parsed was encountered.
 */
class FormatSyntaxError : public SyntaxError {
public:

    /**
     * Create an exception the exception with a default message.
     */
    FormatSyntaxError(LSST_EARGS_TYPED) 
        : SyntaxError(LSST_EARGS_UNTYPED, "Unknonwn syntax error") { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     */
    FormatSyntaxError(LSST_EARGS_TYPED, const std::string& msg) 
        : SyntaxError(LSST_EARGS_UNTYPED, msg) { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     * @param lineno  a line number in the file (or stream) being parsed 
     *                  where the problem occurred.  The first line of the 
     *                  file is typically line 1.  
     */
    FormatSyntaxError(LSST_EARGS_TYPED, const std::string& msg, int lineno) 
        : SyntaxError(LSST_EARGS_UNTYPED, msg, lineno) { }
};

/**
 * an exception thrown because syntax was encountered that is legal for the 
 * format being parsed but which is not supported for encoding Policies.  
 */
class UnsupportedSyntax : public SyntaxError {
public:

    /**
     * Create an exception the exception with a default message.
     */
    UnsupportedSyntax(LSST_EARGS_TYPED) 
        : SyntaxError(LSST_EARGS_UNTYPED, "Unsupported syntax error") { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     */
    UnsupportedSyntax(LSST_EARGS_TYPED, const std::string& msg) 
        : SyntaxError(LSST_EARGS_UNTYPED, msg) { }

    /**
     * Create an exception the exception with a given message.
     * @param msg     a message describing the problem.
     * @param lineno  a line number in the file (or stream) being parsed 
     *                  where the problem occurred.  The first line of the 
     *                  file is typically line 1.  
     */
    UnsupportedSyntax(LSST_EARGS_TYPED, const std::string& msg, int lineno) 
        : SyntaxError(LSST_EARGS_UNTYPED, msg, lineno) { }
};




}}}  // end namespace lsst::pex::policy


#endif // LSST_PEX_POLICY_PARSER_EXCEPTIONS_H
