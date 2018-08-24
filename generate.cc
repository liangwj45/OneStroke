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
using std::unordered_set;
using std::vector;

int rand(int beg, int end) {
  static unsigned int let_more_random = 16303050;
  srand((unsigned int)time(nullptr) * let_more_random++);
  return std::rand() % (end - beg + 1) + beg;
}

std::string getTime() {
  time_t cur_time = time(0);
  tm* ltm = localtime(&cur_time);
  return std::to_string(1900 + ltm->tm_year) + "-" +
         std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) +
         "-" + std::to_string(ltm->tm_hour) + "-" +
         std::to_string(ltm->tm_min) + "-" + std::to_string(ltm->tm_sec);
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
  Map(int width, int height, int blank)
      : width_(width),
        height_(height),
        blank_(blank),
        map_(height, vector<bool>(width, true)),
        ways_(height, vector<int>(width, 0)),
        steps_(width * height - blank),
        stop_signal_(false),
        used_(height, vector<bool>(width, false)) {
    do {
      build();
    } while (!stop_signal_);
    computeWays();
  }

  struct Point {
    int x;
    int y;
  };

  vector<vector<bool>> getMap() const { return map_; }

  vector<vector<int>> getWay() const { return ways_; }

  Point getStartPoint() const { return start_point_; }

 private:
  void build() {
    static int count = 0;
    cout << "Trying " << ++count << endl;

    do {
      setBlanks();
      computeWays();
    } while (!checkEndPoint());

    int time = 0;
    do {
      // cout << "Trying " << ++count << endl;
      computeWays();
      do {
        setStartPoint();
      } while (!checkStartPoint());
      stop_signal_ = checkPath();
    } while (!stop_signal_ && ++time < 15);
  }

  bool checkEndPoint() {
    int end = 0;
    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        end += ways_[i][j] == 1;
      }
    }
    return end == 1;
  }

  bool checkStartPoint() {
    int x = start_point_.x;
    int y = start_point_.y;
    return map_[x][y] && ways_[x][y] > 1;
  }

  bool checkPath() {
    init();
    updateWays(start_point_, -1);
    return dfs(1, start_point_);
  }

  void init() {
    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        used_[i][j] = false;
      }
    }
  }

  bool dfs(int step, const Point& point) {
    if (step == steps_) return true;
    int x = point.x;
    int y = point.y;
    // int single = 0;
    // single += x > 0 && !used_[x - 1][y] && ways_[x - 1][y] <= 1;
    // single += y > 0 && !used_[x][y - 1] && ways_[x][y - 1] <= 1;
    // single += x < height_ - 1 && !used_[x + 1][y] && ways_[x + 1][y] <= 1;
    // single += y < width_ - 1 && !used_[x][y + 1] && ways_[x][y + 1] <= 1;
    // if (single > 1) return false;

    if (x > 0 && map_[x - 1][y] && !used_[x - 1][y]) {
      Point next = {x - 1, y};
      used_[x - 1][y] = true;
      // updateWays(next, -1);
      if (dfs(step + 1, next)) return true;
      used_[x - 1][y] = false;
      // updateWays(next, 1);
    }
    if (y > 0 && map_[x][y - 1] && !used_[x][y - 1]) {
      Point next{x, y - 1};
      used_[x][y - 1] = true;
      // updateWays(next, -1);
      if (dfs(step + 1, next)) return true;
      used_[x][y - 1] = false;
      // updateWays(next, 1);
    }
    if (x < height_ - 1 && map_[x + 1][y] && !used_[x + 1][y]) {
      Point next{x + 1, y};
      used_[x + 1][y] = true;
      // updateWays(next, -1);
      if (dfs(step + 1, next)) return true;
      used_[x + 1][y] = false;
      // updateWays(next, 1);
    }
    if (y < width_ - 1 && map_[x][y - 1] && !used_[x][y - 1]) {
      Point next{x, y + 1};
      used_[x][y + 1] = true;
      // updateWays(next, -1);
      if (dfs(step + 1, next)) return true;
      used_[x][y + 1] = false;
      // updateWays(next, 1);
    }
    return false;
  }

  void updateWays(const Point& point, int d) {
    int x = point.x;
    int y = point.y;
    if (x > 0) ways_[x - 1][y] += d;
    if (y > 0) ways_[x][y - 1] += d;
    if (x < height_ - 1) ways_[x + 1][y] += d;
    if (y < width_ - 1) ways_[x][y + 1] += d;
  }

  void setBlanks() {
    // clear
    blanks_.clear();
    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        map_[i][j] = true;
      }
    }

    int t = blank_ / 2;
    while (t) {
      int x = rand(0, height_ - 1);
      int y = 0;
      Point point = {x, y};
      bool flag = false;
      for (auto each : blanks_) {
        if (each.x == x && each.y == y) {
          flag = true;
          break;
        }
      }
      if (flag) continue;
      blanks_.push_back(point);
      map_[x][y] = false;
      --t;
    }

    t = (blank_ + 1) / 2;
    while (t) {
      int x = 0;
      int y = rand(0, width_ - 1);
      Point point = {x, y};
      bool flag = false;
      for (auto each : blanks_) {
        if (each.x == x && each.y == y) {
          flag = true;
          break;
        }
      }
      if (flag) continue;
      blanks_.push_back(point);
      map_[x][y] = false;
      --t;
    }
  }

  void setStartPoint() {
    start_point_.x = rand(0, height_ - 1);
    start_point_.y = rand(0, width_ - 1);
  }

  void computeWays() {
    // clear
    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        ways_[i][j] = 0;
      }
    }

    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        if (!map_[i][j]) continue;
        ways_[i][j] += i > 0 && map_[i - 1][j];
        ways_[i][j] += j > 0 && map_[i][j - 1];
        ways_[i][j] += i < height_ - 1 && map_[i + 1][j];
        ways_[i][j] += j < width_ - 1 && map_[i][j + 1];
      }
    }
  }

 private:
  int width_;
  int height_;
  int blank_;
  vector<vector<bool>> map_;
  vector<vector<int>> ways_;
  Point start_point_;
  int steps_;
  vector<Point> blanks_;
  bool stop_signal_;
  vector<vector<bool>> used_;
};

}  // namespace OneStroke

/**
 * Need two arguments for generating maps
 * 1. difficulty: "Easy", "Normal", "Hard", "Master"
 * 2. number: the amount of maps to generate
 *
 */
int main() {
  using namespace OneStroke;
  std::string difficulty;
  std::cin >> difficulty;
  int width = difficulty == "Easy" ? 4 : 6;
  int height = difficulty == "Easy" || difficulty == "Normal"
                   ? 5
                   : difficulty == "Hard" ? 6 : 7;
  int blank = 4;

  std::string path = "D:\\Files\\code\\OneStroke\\maps_" + getTime() + ".csv";
  std::ofstream file;
  file.open(path.c_str());
  int number;
  std::cin >> number;
  for (int k = 1; k <= number; ++k) {
    cout << "Generating map " << k << "..." << endl;
    Map fmap(width, height, blank);
    auto map = fmap.getMap();
    auto way = fmap.getWay();
    int x = fmap.getStartPoint().x;
    int y = fmap.getStartPoint().y;
    file << k << '\n';
    file << "// map" << '\n';  //
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        file << (map[i][j] ? i == x && j == y ? 2 : 1 : 0);
        if (j < width - 1) file << ',';
      }
      file << '\n';
    }
    file << "// way" << '\n';  //
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        file << way[i][j];
        if (j < width - 1) file << ',';
      }
      file << '\n';
    }
  }
  cout << "Done." << endl;
  file.close();
}