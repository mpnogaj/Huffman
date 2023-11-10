#pragma once
#include <string>

/**
 * @brief Basic interface for communication with the user
 */
class ui
{
  public:
    virtual ~ui() = default;
    virtual void write_message(const std::string &msg) const = 0;
    virtual void app_error(const std::string &error_msg) const = 0;
};

/**
 * @brief Simple console interface. Writes to stdout and stderr
 */
class console_ui final : public ui
{
  public:
    void write_message(const std::string &msg) const override;
    void app_error(const std::string &error_msg) const override;
};
