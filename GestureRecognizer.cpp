#include "GestureRecognizer.h"
#include <limits>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <fstream>
#include <iostream>
GestureRecognizer::GestureRecognizer(int dim, double threshold, double firstThreshold, int maxslope, double minlength)
{
    this->dimonsion = dim;
    this->globalThreshold = threshold;
    this->firstThreshold = firstThreshold;
    this->max_slope = maxslope;
    this->min_length = minlength;
}
void GestureRecognizer::addOrUpdate(const Sequence seq, string name)
{
    this->known_gestures[name] = seq;
}
string GestureRecognizer::recognize(const Sequence &seq)
{
    double minDist = std::numeric_limits<double>::infinity();
    string type = "UNKNOWN";
    map<string, Sequence >::iterator it;
    for(it = known_gestures.begin(); it != known_gestures.end(); it++)
    {
        Sequence example = it->second;
        if(dist2(seq.at(seq.size()-1), example.at(example.size()-1)) < firstThreshold)
        {
            double d = dtw( seq, example) / example.size();
            if( d < minDist)
            {
                minDist = d;
                type = it->first;
            }

        }
    }
 //   std::cout << "DTW" << minDist << std::endl;
    return minDist < globalThreshold ? type : "UNKNOWN";
}

double GestureRecognizer::dist1(const vector<double> &a, const vector<double> &b)
{
    double d = 0;
    for(int i = 0; i < dimonsion; i++)
    {
        d += abs(a[i]-b[i]);
    }
    return d;
}

double GestureRecognizer::dist2(const vector<double> &a, const vector<double> &b)
{
    double d = 0;
    for(int i = 0; i < dimonsion; i++)
    {
        d += pow(a[i] - b[i], 2);
    }
    return sqrt(d);
}

double GestureRecognizer::dtw(const Sequence &seq1, const Sequence &seq2)
{
    Sequence seq1r(seq1);
    reverse(seq1r.begin(), seq1r.end());
    Sequence seq2r(seq2);
    reverse(seq2r.begin(), seq2r.end());

    double tab[seq1r.size()+1][seq2r.size()+1];
    double slopeI[seq1r.size()+1][seq2r.size()+1];
    double slopeJ[seq1r.size()+1][seq2r.size()+1];

    for(int i = 0; i < seq1r.size()+1; i++)
    {
        for(int j = 0; j < seq2r.size()+1; j++)
        {
            tab[i][j] = numeric_limits<double>::infinity();
            slopeI[i][j] = 0;
            slopeJ[i][j] = 0;
        }
    }
    tab[0][0] = 0;

    //Dynamic computation of the DTW matrix
    for(int i = 1; i < seq1r.size()+1; i++)
    {
        for(int j = 1; j < seq2r.size()+1; j++)
        {
            if(tab[i][j-1] < tab[i-1][j-1] && tab[i][j-1] < tab[i-1][j] && slopeI[i][j-1] < max_slope)
            {
                tab[i][j] = dist2(seq1r[i-1], seq2r[j-1]) + tab[i][j-1];
                slopeI[i][j] = slopeJ[i][j-1] + 1;
                slopeJ[i][j] = 0.0;
            }
            else if(tab[i-1][j] < tab[i-1][j-1] && tab[i-1][j] < tab[i][j-1] && slopeJ[i-1][j] < max_slope)
            {
                tab[i][j] = dist2(seq1r[i-1], seq2r[j-1]) + tab[i-1][j];
                slopeI[i][j] = 0;
                slopeJ[i][j] = slopeJ[i-1][j] + 1;
            }
            else
            {
                tab[i][j] = dist2(seq1r[i-1], seq2r[j-1]) + tab[i-1][j-1];
                slopeI[i][j] = 0;
                slopeJ[i][j] = 0;
            }
        }
    }

    //find best between seq2 and an ending(postfix) of seq1
    double bestMatch = numeric_limits<double>::infinity();
    for(int i = 1; i < seq1r.size()+1-min_length; i++)
    {
        if(tab[i][seq2r.size()] < bestMatch)
        {
            bestMatch = tab[i][seq2r.size()];
        }
    }
    return bestMatch;
}

bool GestureRecognizer::saveGestures(const char* filename)
{
    std::ofstream f(filename);
    Sequence::iterator it;
    map<string, Sequence >::iterator ges_it;
    for(ges_it = known_gestures.begin(); ges_it != known_gestures.end(); ges_it++){
        string name = ges_it->first;
        Sequence gesture = ges_it->second;
        f << name << "\n";
        f << gesture.size() << "\n";
        for(it = gesture.begin(); it != gesture.end(); it++){
            for(int i = 0; i < SKELETON_INFO_SIZE; i++){
                f << (*it)[i] << " ";
            }
            f << "\n";
        }
    }
    return true;
}
bool GestureRecognizer::loadGestures(const char *filename)
{
    std::ifstream f(filename);
    if(f.fail()){
        std::cout << "fail to load gestures" << std::endl;
        return false;
    }
    string gesturename;
    Sequence gesture;
    int size;
    while(!f.eof()){
        f >> gesturename;
        f >> size;
        vector<double> frame;
        double tmp;
        for(int i = 0; i < size; i++){
            for(int j = 0; j < SKELETON_INFO_SIZE; j++){
                f >> tmp;
                frame.push_back(tmp);
            }
            gesture.push_back(frame);
        }
        addOrUpdate(gesture, gesturename);
        gesture.clear();
        frame.clear();
    }
    return true;
}
