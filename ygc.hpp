/**
 *  YangGang Coin
 *  http://github.com/fcecin/ygc
 */
#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/transaction.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;

   class [[eosio::contract("ygc")]] token : public contract {
   public:
      using contract::contract;

      [[eosio::action]]
         void create( name   issuer,
                      asset  maximum_supply);

      [[eosio::action]]
         void issue( name to, asset quantity, string memo );

      [[eosio::action]]
         void retire( asset quantity, string memo );

      [[eosio::action]]
         void transfer( name    from,
                        name    to,
                        asset   quantity,
                        string  memo );

      [[eosio::action]]
         void open( name owner, const symbol& symbol, name ram_payer );

      [[eosio::action]]
         void close( name owner, const symbol& symbol );

      [[eosio::action]]
         void claim( name owner ); // Implicit token symbol

      [[eosio::action]]
         void burn( name owner, asset quantity );

      [[eosio::action]]
         void income( name to, asset quantity, string memo );

      struct income_notification_abi {
         name        to;
         asset       quantity;
         string      memo;
      };

      static asset get_supply( name token_contract_account, symbol_code sym_code )
      {
         stats statstable( token_contract_account, sym_code.raw() );
         const auto& st = statstable.get( sym_code.raw() );
         return st.supply;
      }

      static asset get_balance( name token_contract_account, name owner, symbol_code sym_code )
      {
         accounts accountstable( token_contract_account, owner.value );
         const auto& ac = accountstable.get( sym_code.raw() );
         return ac.balance;
      }

      using create_action = eosio::action_wrapper<"create"_n, &token::create>;
      using issue_action = eosio::action_wrapper<"issue"_n, &token::issue>;
      using retire_action = eosio::action_wrapper<"retire"_n, &token::retire>;
      using transfer_action = eosio::action_wrapper<"transfer"_n, &token::transfer>;
      using open_action = eosio::action_wrapper<"open"_n, &token::open>;
      using close_action = eosio::action_wrapper<"close"_n, &token::close>;
      using claim_action = eosio::action_wrapper<"claim"_n, &token::claim>;
      using burn_action = eosio::action_wrapper<"burn"_n, &token::burn>;
      using income_action = eosio::action_wrapper<"income"_n, &token::income>;
   private:
      static constexpr symbol COIN_SYMBOL = symbol("YANG", 4);
         
      typedef uint16_t time_type;
         
      struct [[eosio::table]] account {
         asset       balance;
         time_type   last_claim_day;
            
         uint64_t primary_key()const { return balance.symbol.code().raw(); }
      };

      struct [[eosio::table]] currency_stats {
         asset    supply;
         asset    max_supply;
         name     issuer;

         uint64_t primary_key()const { return supply.symbol.code().raw(); }
      };

      typedef eosio::multi_index< "accounts"_n, account > accounts;
      typedef eosio::multi_index< "stat"_n, currency_stats > stats;

      void sub_balance( name owner, asset value );
      void add_balance( name owner, asset value, name ram_payer );

      void try_ubi_claim( name from, const symbol& sym, name payer, stats& statstable, const currency_stats& st, bool fail );

      void log_claim( name claimant, asset claim_quantity, time_type next_last_claim_day, time_type lost_days );

      static int64_t get_precision_multiplier ( const symbol& symbol ) {
         int64_t precision_multiplier = 1;
         for (int i=0; i<symbol.precision(); ++i)
            precision_multiplier *= 10;
         return precision_multiplier;
      }

      static string days_to_string( int64_t days );

      static time_type get_today() { return (time_type)(current_time_point().sec_since_epoch() / 86400); }

      static const int64_t max_past_claim_days = 360;

      static const time_type last_signup_reward_day = 0; // Signup rewards off

      static const int64_t units_per_day = 328549; // 32.8549 coins per day, or 1000 per month.
                                                   // This is hard-coded for a token precision of 4.
   };

} /// namespace eosio
