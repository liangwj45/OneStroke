#include <direct.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace OneStroke {

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;

int rand(int beg, int end) {
  static unsigned int let_more_random = 16303050;
  srand((unsigned int)time(nullptr) * let_more_random++);
  return std::rand() % (end - beg + 1) + beg;
}

string getTime() {
  time_t cur_time = time(0);
  tm* ltm = localtime(&cur_time);
  return to_string(1900 + ltm->tm_year) + "-" + to_string(1 + ltm->tm_mon) +
         "-" + to_string(ltm->tm_mday) + "-" + to_string(ltm->tm_hour) + "-" +
         to_string(ltm->tm_min) + "-" + to_string(ltm->tm_sec);
}

class Map {
 public:
  Map(string dif, int number)
      : dif_(dif),
        used_(dif_.height, vector<bool>(dif_.width, false)),
        time_(getTime()),
        number_(number),
        stop_signal_(false) {
    char current_dir[40];
    getcwd(current_dir, 40);
    sprintf(path_, "%s\\data\\%s", current_dir, time_.c_str());
    _mkdir(path_);
    build();
  }

  struct Point {
    int x;
    int y;
  };

  /**
   * ******************************************
   *         Easy    Normal    Hard    Master
   * width   4       6         6       6
   * height  5       5         6       7
   * blank   4       4         4       4
   * ******************************************
   */
  struct Difficulty {
    Difficulty(string dif) : dif(dif) {
      height = dif == "Easy" || dif == "Normal" ? 5 : dif == "Hard" ? 6 : 7;
      width = dif == "Easy" ? 4 : 6;
      blank = 4;
      step = height * width - blank;
    }
    string dif;
    int height;
    int width;
    int blank;
    int step;
  };

 private:
  Difficulty dif_;
  vector<vector<bool>> used_;
  string time_;
  int number_;
  bool stop_signal_;
  char path_[50];
  Point start_point_;
  Point end_point_;

 private:
  void build() {
    cout << "Building map " << 1 << "..." << endl;
    int width = dif_.width;
    int height = dif_.height;
    stop_signal_ = false;

    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        start_point_.x = x;
        start_point_.y = y;
        used_[start_point_.x][start_point_.y] = true;
        nextStep(1, start_point_);
        used_[start_point_.x][start_point_.y] = false;
        if (stop_signal_) break;
      }
      if (stop_signal_) break;
    }

    // do {
    //   start_point_.x = 0;
    //   start_point_.y = rand(0, width - 1);
    //   used_[start_point_.x][start_point_.y] = true;
    //   nextStep(1, start_point_);
    //   used_[start_point_.x][start_point_.y] = false;

    //   start_point_.x = rand(0, height - 1);
    //   start_point_.y = width - 1;
    //   used_[start_point_.x][start_point_.y] = true;
    //   nextStep(1, start_point_);
    //   used_[start_point_.x][start_point_.y] = false;

    //   start_point_.x = height - 1;
    //   start_point_.y = rand(0, width - 1);
    //   used_[start_point_.x][start_point_.y] = true;
    //   nextStep(1, start_point_);
    //   used_[start_point_.x][start_point_.y] = false;

    //   start_point_.x = rand(0, height - 1);
    //   start_point_.y = 0;
    //   used_[start_point_.x][start_point_.y] = true;
    //   nextStep(1, start_point_);
    //   used_[start_point_.x][start_point_.y] = false;
    // } while (!stop_signal_);

    cout << "Done." << endl;
  }

  void nextStep(int step, const Point& point) {
    if (stop_signal_) return;
    if (step == dif_.step) {
      end_point_ = point;
      exportMap();
      return;
    }
    int x = point.x;
    int y = point.y;
    if (x > 0 && !used_[x - 1][y]) {
      Point nextPoint = {x - 1, y};
      if (dis(nextPoint, start_point_) > 1 || step == 1) {
        used_[x - 1][y] = true;
        nextStep(step + 1, nextPoint);
        used_[x - 1][y] = false;
      }
    }
    if (y > 0 && !used_[x][y - 1]) {
      Point nextPoint{x, y - 1};
      if (dis(nextPoint, start_point_) > 1 || step == 1) {
        used_[x][y - 1] = true;
        nextStep(step + 1, nextPoint);
        used_[x][y - 1] = false;
      }
    }
    if (x < dif_.height - 1 && !used_[x + 1][y]) {
      Point nextPoint{x + 1, y};
      if (dis(nextPoint, start_point_) > 1 || step == 1) {
        used_[x + 1][y] = true;
        nextStep(step + 1, nextPoint);
        used_[x + 1][y] = false;
      }
    }
    if (y < dif_.width - 1 && !used_[x][y + 1]) {
      Point nextPoint{x, y + 1};
      if (dis(nextPoint, start_point_) > 1 || step == 1) {
        used_[x][y + 1] = true;
        nextStep(step + 1, nextPoint);
        used_[x][y + 1] = false;
      }
    }
  }

  int dis(const Point& p1, const Point& p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
  }

  void exportMap() {
    static int count = 0;
    char path[50];
    sprintf(path, "%s\\%s_%d.csv", path_, dif_.dif.c_str(), ++count);
    std::ofstream file(path, std::ios::out);
    int x = end_point_.x;
    int y = end_point_.y;
    for (int i = 0; i < dif_.height; ++i) {
      for (int j = 0; j < dif_.width; ++j) {
        file << (used_[i][j] ? i == x && j == y ? 2 : 1 : 0);
      }
    }
    file.close();
    if (count > number_) {
      stop_signal_ = true;
      return;
    }
    cout << "Building map " << count << "..." << endl;
  }
};

}  // namespace OneStroke

int main() {
  int number;
  std::string difficulty;
  std::cin >> difficulty >> number;
  OneStroke::Map map(difficulty, number);
}