#pragma once

/**
 * \file userPoll.h
 *
 * \date Aug 4, 2014
 **/

#include <QtCore/QString>
#include <QtCore/QHash>
#include "calculationStats.h"

using corecvs::Statistics;

class PollModifier
{
public:
    enum Action {
        SET,
        INC,
        DEC
    };

    static inline const char *getName(const Action &value)
    {
        switch (value)
        {
         case SET : return "SET"; break ;
         case INC : return "INC"; break ;
         case DEC : return "DEC"; break ;
         default : return "Not in range"; break ;
        }
        return "Not in range";
    }

    QString key;
    Action action;
    double value;

};

class UserPoll
{
public:
    QHash<QString, double> values;

    void modifyPoll (const PollModifier &modifier)
    {
        if (!values.contains(modifier.key))
        {
            values[modifier.key] = 0.0;
        }

        switch (modifier.action) {
            case PollModifier::SET: values[modifier.key]  = modifier.value; break;
            case PollModifier::INC: values[modifier.key] += modifier.value; break;
            case PollModifier::DEC: values[modifier.key] -= modifier.value; break;
        }
    }

};

typedef QHash<QString, UserPoll> UserPolls;

/* EOF */
