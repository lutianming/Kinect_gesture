#ifndef GESTURERECOGNIZER_H
#define GESTURERECOGNIZER_H
#include <map>
#include <vector>
#include <string>
#include <limits>
using namespace std;
const string GESTURE_UNKNOWN = "UNKNOWN";
const string GESTURE_LEFT_SWIPE_LEFT = "LEFT_SWIPE_LEFT";
const string GESTURE_LEFT_SWIPE_RIGHT = "LEFT_SWIPE_RIGHT";
const string GESTURE_RIGHT_SWIPE_LEFT = "RIGHT_SWIPE_LEFT";
const string GESTURE_RIGHT_SWIPE_RIGHT = "RIGHT_SWIPE_RIGHT";
const int SKELETON_INFO_SIZE = 12; //the 6 X and 6 Y coordinates of hands, elbows and shoulder
typedef vector<vector<double> > Sequence;
class GestureRecognizer
{
public:
    GestureRecognizer(int dim, double threshold, double firstThreshold, int maxslope, double minlength);
    void addOrUpdate(const Sequence seq, string name);
    string recognize(const Sequence &sequence);
    double dtw(const Sequence  &seq1, const Sequence &seq2);
    bool saveGestures(const char* filename);
    bool loadGestures(const char* filename);
    map<string, Sequence >& getGestures(){ return known_gestures;}
private:
    map<string, Sequence > known_gestures;
    //size of observation vectors
    int dimonsion;
    // Maximum DTW distance between an example and a sequence being classified.
    double globalThreshold ;
    // Maximum distance between the last observations of each sequence.
    double firstThreshold;
    //maximum vertical or horizontal steps in a row
    int max_slope;
    double min_length;

    double dist1(const vector<double> &a, const vector<double> &b);
    double dist2(const vector<double> &a, const vector<double> &b);


};

#endif // GESTURERECOGNIZER_H
