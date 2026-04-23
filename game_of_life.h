/**
 * This is the student's part for Conway's Game of Life Simulator.
 * You need to finish all functions marked with "TODO" label.
 * When submitting, you should hand in THIS header file (i.e. game_of_life.h) to OJ to get correct mark.
 *
 * By the way, you SHOULD NOT try to read or write any extra information to the console.
 * Any attempt to hack the OJ or special judger WILL BE voided (and punished if it causes severe results).
 *
 * This is part of the 1st homework of Class A and 2nd homework of Class B. The workload isn't large.
 * We've also prepared a small visualizer for you. Enjoy coding ~
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream>

/**
 * Feel free to define/modify variables here.
 */
inline int row = -1, col = -1;
inline std::set<std::pair<int, int>> live_cells;

/**
 * This function is called at the beginning of every game.
 * Read the initial map of the game from stdin.
 * The format of the input is written in the problem description.
 *
 * TODO: Read the initial map from input and initialize your variables.
 */
inline void Initialize() {
  std::cin >> col >> row;
  std::cin.ignore();
  
  live_cells.clear();
  
  std::string pattern = "";
  std::string line;
  while (std::getline(std::cin, line)) {
    pattern += line;
    if (line.find('!') != std::string::npos) {
      break;
    }
  }
  
  // Parse RLE pattern
  int curr_row = 0;
  int curr_col = 0;
  int num = 0;
  bool has_num = false;
  
  for (size_t i = 0; i < pattern.length(); i++) {
    char c = pattern[i];
    
    if (c == '!') {
      break;
    } else if (c >= '0' && c <= '9') {
      num = num * 10 + (c - '0');
      has_num = true;
    } else if (c == 'b') {
      int count = has_num ? num : 1;
      curr_col += count;
      num = 0;
      has_num = false;
    } else if (c == 'o') {
      int count = has_num ? num : 1;
      for (int j = 0; j < count; j++) {
        live_cells.insert({curr_row, curr_col});
        curr_col++;
      }
      num = 0;
      has_num = false;
    } else if (c == '$') {
      int count = has_num ? num : 1;
      curr_row += count;
      curr_col = 0;
      num = 0;
      has_num = false;
    }
  }
}

/**
 * This function is called once in every round of the game.
 * You need to simulate a round of game in this program.
 *
 * Note: You don't need to input or output anything. Just change the variables you defined so that they're in the new round.
 *
 * TODO: Simulate a new round of the game.
 */
inline void Tick() {
  std::map<std::pair<int, int>, int> neighbor_count;
  
  // Count neighbors for all cells that might change
  for (const auto& cell : live_cells) {
    int r = cell.first;
    int c = cell.second;
    
    for (int dr = -1; dr <= 1; dr++) {
      for (int dc = -1; dc <= 1; dc++) {
        if (dr == 0 && dc == 0) continue;
        
        int nr = r + dr;
        int nc = c + dc;
        
        if (nr >= 0 && nr < row && nc >= 0 && nc < col) {
          neighbor_count[{nr, nc}]++;
        }
      }
    }
  }
  
  std::set<std::pair<int, int>> new_live_cells;
  
  // Apply rules
  for (const auto& entry : neighbor_count) {
    int r = entry.first.first;
    int c = entry.first.second;
    int count = entry.second;
    
    bool is_alive = live_cells.count({r, c}) > 0;
    
    if (is_alive) {
      if (count == 2 || count == 3) {
        new_live_cells.insert({r, c});
      }
    } else {
      if (count == 3) {
        new_live_cells.insert({r, c});
      }
    }
  }
  
  live_cells = new_live_cells;
}

/**
 * This function may be called at any time during the game.
 * You need to print the map of the game in a certain format.
 * Output format is written in the problem description.
 *
 * Note: In this function, you just needs to print the map. DO NOT simulate a new round of game.
 *
 * TODO: Print the whole map.
 */
inline void PrintGame() {
  std::cout << col << " " << row << std::endl;
  
  if (live_cells.empty()) {
    std::cout << "!" << std::endl;
    return;
  }
  
  // Find last row with live cells
  int last_row = -1;
  for (const auto& cell : live_cells) {
    if (cell.first > last_row) {
      last_row = cell.first;
    }
  }
  
  std::ostringstream oss;
  
  for (int r = 0; r <= last_row; r++) {
    // Find last alive cell in this row
    int last_alive_col = -1;
    for (const auto& cell : live_cells) {
      if (cell.first == r && cell.second > last_alive_col) {
        last_alive_col = cell.second;
      }
    }
    
    if (last_alive_col == -1) {
      // Empty row - just output $
      oss << "$";
      continue;
    }
    
    // Output this row
    int c = 0;
    while (c <= last_alive_col) {
      bool is_alive = live_cells.count({r, c}) > 0;
      
      // Count consecutive cells of same type
      int count = 1;
      while (c + count <= last_alive_col) {
        bool next_alive = live_cells.count({r, c + count}) > 0;
        if (next_alive != is_alive) break;
        count++;
      }
      
      // Don't output trailing dead cells
      if (!is_alive && c + count > last_alive_col + 1) {
        count = last_alive_col + 1 - c;
        if (count <= 0) break;
      }
      
      if (count > 1) {
        oss << count;
      }
      oss << (is_alive ? 'o' : 'b');
      
      c += count;
    }
    
    // Add $ if not the last row
    if (r < last_row) {
      oss << "$";
    }
  }
  
  // Compress consecutive $ symbols
  std::string pattern = oss.str();
  std::string compressed;
  int i = 0;
  while (i < pattern.length()) {
    if (pattern[i] == '$') {
      int count = 1;
      while (i + count < pattern.length() && pattern[i + count] == '$') {
        count++;
      }
      if (count > 1) {
        compressed += std::to_string(count);
      }
      compressed += '$';
      i += count;
    } else {
      compressed += pattern[i];
      i++;
    }
  }
  
  compressed += "!";
  std::cout << compressed << std::endl;
}

/**
 * This function may be called at any time during the game.
 * You need to return the number of live cells.
 *
 * @returns the number of live cells in the map.
 *
 * Note: In this function, you SHOULD NOT simulate a new round of game or print anything to stdout.
 *
 * TODO: Returns the number of live cells.
 */
inline int GetLiveCell() {
  return live_cells.size();
}
