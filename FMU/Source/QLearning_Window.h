#ifndef QLEARNING_WINDOW_H
#define QLEARNING_WINDOW_H

#include <vector>
#include "QLearning.h"

class QLearning_Window : public QLearning
{
    public:
      QLearning_Window();

      double learn();
      void reset();
      void setId(const int id);

    private:

      std::string reward_name;
      std::string action_name;
      std::string state_name;
      std::string previous_state_name;
      std::string temp_name;
      std::string pmv_name;

};

#endif // QLEARNING_WINDOW_H
