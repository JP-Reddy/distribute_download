from flask import Flask, flash, redirect, render_template, request, session, abort
import os
import datetime
import wget

app = Flask(__name__)
 
@app.route("/")
def index():
    return "Flask App!"
 
@app.route("/download")
def distributed_download():
    url = request.args.get('url')
    print(url)
    a = datetime.datetime.now().replace(microsecond=0)
    os.system("./master "+url)
    b = datetime.datetime.now().replace(microsecond=0)
    time_taken=(b-a)
    print(time_taken)
    return render_template(
        'test.html',time_taken=time_taken,type="Distributed download")

@app.route("/direct_download")
def simple_download():
    url = request.args.get('url')
    print(url)
    a = datetime.datetime.now().replace(microsecond=0)
    wget.download(url)
    b = datetime.datetime.now().replace(microsecond=0)
    time_taken=(b-a)
    print(time_taken)
    return render_template(
        'test.html',time_taken=time_taken,type="Simple Download")

 
if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)