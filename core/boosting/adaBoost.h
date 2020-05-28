/**
 * \file adaBoost.h
 * \brief This file implements the data structures and algorithms needed for AdaBoost
 *
 * More information on this
 * <ul>
 *   <li>http://www.site.uottawa.ca/~stan/csi5387/boost-tut-ppr.pdf
 *   <li>http://en.wikipedia.org/wiki/AdaBoost
 * </ul>
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#ifndef ADABOOST_H_
#define ADABOOST_H_

#include <fstream>
#include <string>
#include <vector>

#include "utils/global.h"

#include "math.h"

namespace corecvs {

using std::string;
using std::vector;
using std::fstream;

/**
 *  Input data element of the training set for AdaBoost
 *
 *
 **/
template <typename InputType>
class AdaBoostInputData
{
public:

    InputType x;    /**< The input */
    bool y;         /**< Class to which input belongs */
    double weight;  /**< Current weight  */

    AdaBoostInputData(const InputType &_x, bool _y) : x(_x), y(_y)
    {
        weight = 0.0;
    };
};

/**
 * This template will hold the whole vector of inputs. The complete training sets
 *
 *
 * */
template <typename InputType>
class AdaBoostInput : public vector<AdaBoostInputData<InputType> >
{
public:
    /**
     *  This function initializes the weights of the training set elements
     *
     *  \param positiveWeight
     *  \param negativeWeight
     **/
    void initWeigths(double positiveWeight, double negativeWeight)
    {
        for (unsigned i = 0; i < this->size(); i++)
        {
            AdaBoostInputData<InputType> *input = &(this->at(i));
            input->weight = (input->y) ? positiveWeight : negativeWeight;
        }
        normalizeWeigths();
    }

    /**
     *  Adjusts the weights of the input set to make the total sum equal to 1.0
     **/
    void normalizeWeigths()
    {
        double sumWeights = 0.0;
        for (unsigned i = 0; i < this->size(); i++)
        {
            sumWeights += this->at(i).weight;
        }

        for (unsigned i = 0; i < this->size(); i++)
        {
            this->at(i).weight /= sumWeights;
        }
    }
};

/**
 *  The statistical result of the classifier
 **/
class ClassifierStatistics
{
public:
    unsigned truePositive;
    unsigned trueNegative;
    unsigned falsePositive;
    unsigned falseNegative;

    ClassifierStatistics()
    {
        this->truePositive = 0;
        this->trueNegative = 0;
        this->falsePositive = 0;
        this->falseNegative = 0;
    }

    void addValue(bool predicted, bool real)
    {
        if (!predicted)
        {
            if (real)
                falseNegative++;
            else
                trueNegative++;
        } else {
            if (real)
                truePositive++;
            else
                falsePositive++;
        }
    }

    ~ClassifierStatistics(){};
};

template <typename _InputType>
class Classifier
{
public:
    typedef _InputType InputType;

    bool classify (const _InputType &) const
    {
        return false;
    };
};

/**
 * This class contains the boosted classifier
 *
 **/
template <typename _SimpleClassifierType>
class AdaBoostedClassifier : public Classifier<typename _SimpleClassifierType::InputType>
{
    typedef _SimpleClassifierType SimpleClassifierType;
public:
    vector<SimpleClassifierType *> children;
    vector<double> weights;

    void addClassifier( SimpleClassifierType *child, double weight)
    {
        children.push_back(child);
        weights.push_back(weight);
    }

    bool classify (const typename AdaBoostedClassifier::InputType &input) const
    {
        double result = 0;
        for (unsigned i = 0; i < children.size(); i++)
        {
            SimpleClassifierType *child = children[i];
            /*TODO: replace ? operator with branchless mask */
            result += weights[i] * (child->classify(input) ?  1 : -1);
        }
        return (result > 0);
    }

    unsigned getSize()
    {
        return (unsigned)children.size();
    }
};

/**
 *  This class describes the weak hypothesis generator
 **/
template <typename _Classifier>
class WeakHypothesisGenerator
{
    typedef _Classifier Classifier;
    typedef typename Classifier::InputType InputType;
public:
    Classifier *getBestClassifier(const AdaBoostInput<InputType> &/*inputs*/, double * /*cost*/)
    {
        return NULL;
    };
};

/**
 *  \brief Template for generic AdaBoost algorithm
 **/
template <
        typename ClassifierGenerator,
        typename RealBoostedClassifier = AdaBoostedClassifier<typename ClassifierGenerator::Classifier> >
class AdaBoost
{
    typedef typename ClassifierGenerator::Classifier            SimpleClassifierType;
    typedef typename ClassifierGenerator::InputType             InputType;

    AdaBoost(){};

    int maxIterations;
    double errorPersent;


    AdaBoostedClassifier<SimpleClassifierType> *getBoostedClassifier
        ( AdaBoostInput<InputType> &inputs, ClassifierGenerator &generator )
    {
        AdaBoostedClassifier<SimpleClassifierType> *classifier = new
                AdaBoostedClassifier<SimpleClassifierType>();

        for (int iteration = 0; iteration < maxIterations; iteration++)
        {
            double classifierCost;

            SimpleClassifierType *simpleClassifier =
                   generator.getBestClassifier(inputs, &classifierCost);

            double alpha = 0.5 * log ((1.0 - classifierCost) / classifierCost);
            double alphaExp = exp(alpha);

            /*Regenerate the results*/
            ClassifierStatistics stats;
            typename AdaBoostInput<InputType>::iterator it;

            for (it = inputs.begin(); it != inputs.end(); it++)
            {
                bool prediction = simpleClassifier->classify((*it).x);
                /* If guess was wrong we will increase the weight */
//                double alphaRes = (prediction == (*it).y) ? (1.0f / alphaExp) : (alphaExp);

                (*it).weight *= alphaExp;
                stats.addValue(prediction, (*it).y);
            }
            /*Inputs are corrected now*/
            inputs.normalizeWeigths();


    //        printf("Result so far TP=%d TN=%d FP=%d FN=%d\n", stats.truePositive, stats.trueNegative, stats.falsePositive, stats.falseNegative);
            classifier->addClassifier(simpleClassifier, alpha);


            /*Test for total boosted*/
            ClassifierStatistics totalStats;


            fstream outbreaks;
            outbreaks.open("outbreaks", fstream::out);
            for (unsigned i = 0; i < inputs.size(); i++)
            {
                bool isPositive = inputs[i].y;
                bool prediction = classifier->classify(inputs[i].x);
                totalStats.addValue(prediction, isPositive);

                if (isPositive != prediction)
                {
                    if (isPositive)
                        outbreaks << "FN " << i << "\n";
                    else
                        outbreaks << "FP " << i << "\n";
                }
            }
            outbreaks.close();
            printf("Boosted Result %" PRISIZE_T " classifiers so far TP=%d TN=%d FP=%d FN=%d\n",
                   classifier->children.size(),
                   totalStats.truePositive, totalStats.trueNegative,
                   totalStats.falsePositive, totalStats.falseNegative);

            if ((double)(totalStats.falsePositive + totalStats.falseNegative) / inputs.size() < errorPersent )
                break;
       }
    }


    virtual ~AdaBoost(){}
};


} //namespace corecvs
#endif /* ADABOOST_H_ */

