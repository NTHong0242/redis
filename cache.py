import redis

r = redis.Redis(host='localhost', port=6379, db=0, decode_responses=True)

def get_cache(key):
    return r.get(key)

def set_cache(key, value, ex=60):
    r.set(key, value, ex=ex)  # ex: expiration time in seconds
