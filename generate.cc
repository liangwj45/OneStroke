#include <direct.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

namespace OneStroke {

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::unordered_set;
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
  /**
   * ******************************************
   *         Easy    Normal    Hard    Master
   * width   4       6         6       6
   * height  5       5         6       7
   * blank   4       4         4       4
   * ******************************************
   */
  Map(int width, int height, int blank, int number)
      : width_(width),
        height_(height),
        blank_(blank),
        map_(height, vector<bool>(width, true)),
        steps_(width * height - blank),
        used_(height, vector<bool>(width, false)),
        number_(number),
        stop_signal_(false) {
    cout << "Building map " << 1 << "..." << endl;
    build();
  }

  struct Point {
    int x;
    int y;
  };

 private:
  int width_;
  int height_;
  int blank_;
  vector<vector<bool>> map_;
  Point start_point_;
  int steps_;
  vector<vector<bool>> used_;
  int number_;
  bool stop_signal_;

 private:
  void exportMap() {
    static int count = 0;
    char path[50];
    char dir[40];
    getcwd(dir, 40);
    sprintf(path, "%s\\data\\%s\\maps_%d.csv", dir, getTime(), ++count);
    std::ofstream file(path, std::ios::out);
    int x = start_point_.x;
    int y = start_point_.y;
    file << "map " << count << ":\n";
    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        file << (map_[i][j] ? i == x && j == y ? 2 : 1 : 0);
        if (j < width_ - 1) file << ',';
      }
      file << '\n';
    }
    file.close();
    if (count >= number_) {
      stop_signal_ = true;
      return;
    }
    cout << "Building map " << count << "..." << endl;
  }

  void nextStep(int step, const Point& point) {
    if (stop_signal_) return;
    if (step == steps_) {
      exportMap();
      return;
    }
    int x = point.x;
    int y = point.y;
    if (x > 0 && !used_[x - 1][y] && map_[x - 1][y]) {
      Point nextPoint = {x - 1, y};
      used_[x - 1][y] = true;
      nextStep(step + 1, nextPoint);
      used_[x - 1][y] = false;
    }
    if (y > 0 && !used_[x][y - 1] && map_[x][y - 1]) {
      Point nextPoint{x, y - 1};
      used_[x][y - 1] = true;
      nextStep(step + 1, nextPoint);
      used_[x][y - 1] = false;
    }
    if (x < height_ - 1 && !used_[x + 1][y] && map_[x + 1][y]) {
      Point nextPoint{x + 1, y};
      used_[x + 1][y] = true;
      nextStep(step + 1, nextPoint);
      used_[x + 1][y] = false;
    }
    if (y < width_ - 1 && !used_[x][y - 1] && map_[x][y - 1]) {
      Point nextPoint{x, y + 1};
      used_[x][y + 1] = true;
      nextStep(step + 1, nextPoint);
      used_[x][y + 1] = false;
    }
  }

  void build() {
    start_point_.x = 0;
    start_point_.y = rand(0, width_ - 1);
    nextStep(1, start_point_);

    start_point_.x = rand(0, height_ - 1);
    start_point_.y = width_ - 1;
    nextStep(1, start_point_);

    start_point_.x = height_ - 1;
    start_point_.y = rand(0, width_ - 1);
    nextStep(1, start_point_);

    start_point_.x = rand(0, height_ - 1);
    start_point_.y = 0;
    nextStep(1, start_point_);
  }
};

}  // namespace OneStroke

/**
 * Need two arguments for generating maps
 * 1. difficulty: "Easy", "Normal", "Hard", "Master"
 * 2. number: the amount of maps to generate
 *
 */
int main() {
  std::string difficulty;
  std::cin >> difficulty;
  int width = difficulty == "Easy" ? 4 : 6;
  int height = difficulty == "Easy" || difficulty == "Normal"
                   ? 5
                   : difficulty == "Hard" ? 6 : 7;
  int blank = 4;
  int number;
  std::cin >> number;
  OneStroke::Map map(width, height, blank, number);
}