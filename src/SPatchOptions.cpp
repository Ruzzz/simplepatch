//
// Project: Substitute
// Date:    2013-03-28
// Author:  Ruzzz <ruzzzua[]gmail.com>
//


#include "SPatchOptions.h"


namespace SPatch {


//
//  Error
//


const char *SPatch::Error::toString() const
{
    switch (code_)
    {
        case OK:                            return "OK";

        case WRONG_ARGUMENTS_NUMBER:        return "Wrong number of arguments";
        case INVALID_ARGUMENT:              return invalidArgumentError_.c_str();

        default:                            return "Unknown error";
    }
}


bool operator==(const Error &lhs, const Error &rhs)
{
    return lhs.code() == rhs.code();
}


bool operator==(const Error &lhs, const Error::Code &rhs)
{
    return lhs.code() == rhs;
}


bool operator==(const Error::Code &lhs, const Error &rhs)
{
    return lhs == rhs.code();
}


}  // namespace SPatch
