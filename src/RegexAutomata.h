#ifndef REGEX_AUTOMATA_H
#define REGEX_AUTOMATA_H

#include <string>
#include <vector>
#include <memory>

namespace regex
{
    namespace automata
    {
        struct CharRange
        {
            int first_;
            int last_;

            CharRange(int first, int last) : first_(first), last_(last) { }
        };

        struct StateMachine;

        // Base state class
        struct State
        {
            // Accept state or not
            bool accept_;

            // Next states
            std::vector<const State *> next_;

            explicit State(std::size_t next_state_count)
                : accept_(false),
                  next_(next_state_count)
            {
            }

            virtual ~State()
            {
            }

            const State * GetNextState(const StateMachine *state_machine,
                                       const char *&current, const char *&end) const;

            // Complete this state, then call 'GetNextState' to get next state
            // when first of return value is true, if second of return value is
            // existed, then use second value as next state.
            virtual std::pair<bool, const State *> CompleteState(const char *&current,
                                                                 const char *&end) const;
        };

        struct StateMachine
        {
            // Character set
            std::vector<CharRange> char_set_;
            // All states
            std::vector<std::unique_ptr<State>> states_;
            // Start state of the state machine
            const State *start_state_;

            StateMachine() : start_state_(nullptr) { }

            StateMachine(const StateMachine &) = delete;
            void operator = (const StateMachine &) = delete;

            // Get index of next states in State by character 'c' as
            // second value of return pair when first value of return
            // pair is true.
            std::pair<bool, std::size_t> GetNextStateIndex(int c) const;
        };

        // Repeat state class
        struct RepeatState : public State
        {
            int repeat_min_;
            int repeat_max_;
            std::unique_ptr<StateMachine> sub_state_machine_;

            // If direct next state existed, then state machine
            // move to direct next state when current state is
            // completely.
            const State *direct_next_;

            RepeatState(std::size_t next_state_count,
                        int repeat_min, int repeat_max,
                        std::unique_ptr<StateMachine> sub_state_machine)
                : State(next_state_count),
                  repeat_min_(repeat_min),
                  repeat_max_(repeat_max),
                  sub_state_machine_(std::move(sub_state_machine)),
                  direct_next_(nullptr)
            {
            }

            virtual std::pair<bool, const State *> CompleteState(const char *&current,
                                                                 const char *&end) const;
        };

        std::unique_ptr<StateMachine> ConstructStateMachine(const std::string &re);
    } // namespace automata
} // namespace regex

#endif // REGEX_AUTOMATA_H
