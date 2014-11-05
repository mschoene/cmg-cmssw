#ifndef MT2Sample_h
#define MT2Sample_h

#include <string>
#include <vector>




class MT2Sample {


 public:

  MT2Sample();
  ~MT2Sample();


  static std::vector<MT2Sample> loadSamples(const std::string& filename);

  // publica data members:
  std::string name;
  std::string sname;
  std::string dir;
  std::string file;
  int id;
  float nevents;
  float xsection;
  float lumi;
  float kfact;
  float filter;
  float scale1fb;
  float PU_avg_weight;

 private:


};

#endif
