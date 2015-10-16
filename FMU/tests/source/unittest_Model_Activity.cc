

#include <limits.h>
#include "Model_Activity.h"
#include "SimulationConfig.h"
#include "Utility.h"

#include "gtest/gtest.h"


TEST(Activity, Dissagregate) {
    SimulationConfig::agents.clear();

    SimulationConfig::parseConfiguration("tests/Files/SimulationConfig2.xml");
    SimulationConfig::info.timeSteps = 105182;
    SimulationConfig::info.timeStepsPerHour = 6;

    Model_Activity ma;
    std::vector<double> activities = ma.preProcessActivities(0);
    EXPECT_EQ(activities.at(0), 0);
    EXPECT_EQ(activities.at(1000), 0);
    EXPECT_EQ(activities.at(2000), 2);
    EXPECT_EQ(activities.at(3000), 1);
    EXPECT_EQ(activities.at(4000), 1);
    EXPECT_EQ(activities.at(5000), 9);
    EXPECT_EQ(activities.at(6000), 9);
    bool found = false;
    for(int i = 0; i < 10; i++){
      for(double a : activities){
        if(a == i){
          found = true;
        }
      }
      EXPECT_EQ(found, true);
      found = false;
    }


}

int getActivity(double *p, double drand){
  int activity;
  double sum = 0;
  for(int i =0; i < 10; i++){
    sum += p[i];
    if(sum >= drand)
    {
        activity = i;
        break;
    }
  }
  return activity;
}

TEST(Activity, multinominalRandom) {
  double p[10] = {0.036078751,0.12437013,0.082256370,0.003995250,0.027476964,0.030800,0.114028071,0.0102342340,0.043672692,0.52712584};

  double drand = Utility::randomDouble(0.0,1.0);
  drand = 0.001;
  int activity = getActivity(p,drand);
  ASSERT_EQ(activity, 0);
  drand = 0.11;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 1);
  drand = 0.21;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 2);
  drand = 0.243;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 3);
  drand = 0.25;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 4);
  drand = 0.29;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 5);
  drand = 0.40;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 6);
  drand = 0.4191;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 7);
  drand = 0.43;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 8);
  drand = 0.99999999;
  activity = getActivity(p,drand);
  ASSERT_EQ(activity, 9);


}



TEST(Activity, multinominalActivity) {
  Model_Activity ma;
  double p0[10] = {1,0,0,0,0,0,0,0,0,0};
  ASSERT_EQ(ma.multinominalActivity(p0), 0);
  double p1[10] = {0,1,0,0,0,0,0,0,0,0};
  ASSERT_EQ(ma.multinominalActivity(p1), 1);
  double p2[10] = {0,0,1,0,0,0,0,0,0,0};
  ASSERT_EQ(ma.multinominalActivity(p2), 2);
  double p3[10] = {0,0,0,1,0,0,0,0,0,0};
  ASSERT_EQ(ma.multinominalActivity(p3), 3);
  double p4[10] = {0,0,0,0,1,0,0,0,0,0};
  ASSERT_EQ(ma.multinominalActivity(p4), 4);
  double p5[10] = {0,0,0,0,0,1,0,0,0,0};
  ASSERT_EQ(ma.multinominalActivity(p5), 5);
  double p6[10] = {0,0,0,0,0,0,1,0,0,0};
  ASSERT_EQ(ma.multinominalActivity(p6), 6);
  double p7[10] = {0,0,0,0,0,0,0,1,0,0};
  ASSERT_EQ(ma.multinominalActivity(p7), 7);
  double p8[10] = {0,0,0,0,0,0,0,0,1,0};
  ASSERT_EQ(ma.multinominalActivity(p8), 8);
  double p9[10] = {0,0,0,0,0,0,0,0,0,1};
  ASSERT_EQ(ma.multinominalActivity(p9), 9);

  double p[10] = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};

  double top = 10000000;

  double px[10] = {0,0,0,0,0,0,0,0,0,0};
  for(int i = 0; i < top; i++){

      int activity = ma.multinominalActivity(p);
      px[activity] = px[activity] + 1;
  }

  for(int i = 0; i < 10; i++){
    ASSERT_NEAR(px[i] / top, 0.1,0.001);
    px[i] = 0;
  }

  double pp[10] = {0.036078751,0.12437013,0.082256370,0.003995250,0.027476964,0.030800,0.114028071,0.0102342340,0.043672692,0.52712584};

  double sum = 0;
  for(int i = 0; i < 10; i++){
    sum += pp[i];
  }
  ASSERT_NEAR(sum, 1,0.001);

  for(int i = 0; i < top; i++){
      int activity = ma.multinominalActivity(pp);
      px[activity] = px[activity] + 1;
  }

  ASSERT_NEAR(px[0] / top, 0.036,0.001);
  ASSERT_NEAR(px[1] / top, 0.124,0.001);
  ASSERT_NEAR(px[2] / top, 0.082,0.001);
  ASSERT_NEAR(px[3] / top, 0.003,0.001);
  ASSERT_NEAR(px[4] / top, 0.027,0.001);
  ASSERT_NEAR(px[5] / top, 0.030,0.001);
  ASSERT_NEAR(px[6] / top, 0.114,0.001);
  ASSERT_NEAR(px[7] / top, 0.010,0.001);
  ASSERT_NEAR(px[8] / top, 0.043,0.001);
  ASSERT_NEAR(px[9] / top, 0.527,0.001);

}

TEST(Activity, multinominalP) {
    SimulationConfig::agents.clear();

    SimulationConfig::parseConfiguration("tests/Files/SimulationConfig1.xml");

    SimulationConfig::info.timeSteps = 105182;
    SimulationConfig::info.timeStepsPerHour = 6;
    SimulationConfig::info.startDayOfWeek = 1;
    Model_Activity ma;
    std::vector<double> activities = ma.preProcessActivities(0);
    double p[4][7][24][10];
    ma.multinominalP(p,0);
    double sum = 0;
    for(int s = 0; s < 4; s++){
      for(int d = 0; d < 7; d++){
        for(int h = 0; h < 24; h++){
          sum = 0;
          for(int i = 0; i < 10; i++){
            sum += p[s][d][h][i];
          }
          ASSERT_NEAR(sum, 1,0.000000001);
        }
      }
    }

}

TEST(Activity, multinominal) {
    SimulationConfig::agents.clear();

    SimulationConfig::parseConfiguration("tests/Files/SimulationConfig1.xml");

    SimulationConfig::info.timeSteps = 10518;
    SimulationConfig::info.timeStepsPerHour = 6;
    SimulationConfig::info.startDayOfWeek = 1;
    Model_Activity ma;


    std::vector<double> activities = ma.preProcessActivities(0);

    ASSERT_EQ(activities.at(0), 0);
    ASSERT_EQ(activities.at(1000), 0);
    ASSERT_EQ(activities.at(2000), 0);
    ASSERT_EQ(activities.at(3000), 0);
    ASSERT_EQ(activities.at(4000), 0);
    ASSERT_EQ(activities.at(5000), 6);
    ASSERT_EQ(activities.at(6000), 0);
    bool found = false;
    for(int i = 0; i < 3; i++){
      for(double a : activities){
        if(a == i){
          found = true;
        }
      }

      ASSERT_EQ(found, true);
      found = false;
    }
    for(int i = 4; i < 10; i++){
      for(double a : activities){
        if(a == i){
          found = true;
        }
      }

      ASSERT_EQ(found, true);
      found = false;
    }
}
/*
TEST(Appliance, Ownership) {

  Appliance a;
  ASSERT_NEAR(0.932138, a.ownership(),0.001);
  EXPECT_GT(a.ownership(), 0);
  EXPECT_GT(1, a.ownership());

}



TEST(Appliance, switchon) {

  Appliance a;
  a.setAppliance(2);
  //EXPECT_FALSE(a.onAt(1));
  ASSERT_NEAR(0.0308,a.getMeanFraction(),0.001);


}
*/