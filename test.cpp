#include<iostream>
#include <string.h>
#include <libmemcached/memcached.h>
#include <random>
#include <assert.h>

using namespace std;

class KVCache {
  private:
    memcached_st *client_;
    memcached_return cache_return_;
    memcached_server_st *server_;
    int expiration_;
    uint32_t flags_;

    uint32_t hit_, total_;

  public:
    KVCache() : client_(NULL), server_(NULL), expiration_(0), flags_(0), hit_(0), total_(0) {}

    void init() {
      client = memcached_create(NULL);
      server = memcached_server_list_append(server, "localhost", 12345, &cache_return);
      cache_return = memcached_server_push(client, server);
      if (MEMCACHED_SUCCESS != cache_return) {
        cout<<"memcached server push failed! cache return:"<<cache_return<<endl;
      }
    }

    char* get(const string &key) {
      total_++;
      size_t key_len = key.length();
      size_t val_len;
      char *val = memcached_get(client_, key.c_str(), key_len, &val_len, &flags_, &cache_return_);
      if (cache_return_ == MEMCACHED_SUCCESS) {
        hit_++;
        return val;
      } else {
        return NULL;
      }
    }

    void put(const string &key, const string &val) {
      cache_return_ = memcached_set(client_, key.c_str(), key.length(), val.c_str(), val.length(), expiration_, flags_);
      assert(cache_return_ == MEMCACHED_SUCCESS);
    }

    double getHitRatio() {
      return double(hit_) / total_;
    }

};

int main(int argc, char *argv[])
{
  KVCache cache;
  cache.init();
  string key = "cpp_key";
  string val = "cpp_value";
  cache.put(key, val);
  char *val = cache.get(key);
  cout<<"get: "<<val<<endl;
  return 0;
}