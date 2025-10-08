from flask import Flask, jsonify
import requests
from cache import get_cache, set_cache

app = Flask(__name__)

@app.route("/joke")
def get_joke():
    cached_joke = get_cache("joke")

    if cached_joke:
        return jsonify({"joke": cached_joke, "cached": True})

    response = requests.get("https://official-joke-api.appspot.com/random_joke")
    joke = f"{response.json()['setup']} {response.json()['punchline']}"

    set_cache("joke", joke, ex=60)  # cache trong 60 giây

    return jsonify({"joke": joke, "cached": False})

if __name__ == "__main__":
    app.run(debug=True)
