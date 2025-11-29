#pragma once

/// @file transition.h
/// @brief Core data structure for storing RL experience transitions.
/// This file contains the definition of the Transition struct, which is used to
/// store RL experience transitions.

namespace replay_buffer {
/// @brief Represents a single RL experience transition (s, a, r, s', done).
/// @tparam Observation Type of state observations
/// @tparam Action Type of actions taken
template <typename Observation, typename Action>
struct Transition {
  Observation observation;
  Action action;
  float reward;
  Observation next_observation;
  bool done;

  /// @brief Default constructor for Transition.
  Transition() = default;

  /// @brief Constructor for Transition.
  /// @param obs Observation
  /// @param act Action
  /// @param r Reward
  /// @param next_obs Next observation
  /// @param d Done
  Transition(Observation obs, Action act, float r, Observation next_obs, bool d)
      : observation(obs),
        action(act),
        reward(r),
        next_observation(next_obs),
        done(d) {}
};
}  // namespace replay_buffer
