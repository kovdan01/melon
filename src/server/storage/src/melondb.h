// generated by ./ddl2cpp /home/user/Desktop/melondb.sql /home/user/Projects/melon/src/server/storage/src/melondb.h melon
#ifndef MELON_MELONDB_H_H
#define MELON_MELONDB_H_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace melon
{
  namespace Users_
  {
    struct UserId
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "user_id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T userId;
            T& operator()() { return userId; }
            const T& operator()() const { return userId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct Username
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "username";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T username;
            T& operator()() { return username; }
            const T& operator()() const { return username; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Status
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "status";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T status;
            T& operator()() { return status; }
            const T& operator()() const { return status; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::tinyint>;
    };
  } // namespace Users_

  struct Users: sqlpp::table_t<Users,
               Users_::UserId,
               Users_::Username,
               Users_::Status>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "users";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T users;
        T& operator()() { return users; }
        const T& operator()() const { return users; }
      };
    };
  };
} // namespace melon
#endif