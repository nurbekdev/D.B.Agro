from flask import Flask, render_template, redirect, request, make_response, url_for, send_file
from time import time, ctime
from uuid import uuid4
from requests import get
from keras.models import load_model
import tensorflow as tf
from PIL import Image, ImageOps
import numpy as np



app = Flask("__name__")


#model:
np.set_printoptions(suppress=True)
model = load_model("keras_model.h5", compile = False)
class_names = open("labels.txt", "r").readlines()


# def detect_class(file_path):
#     # img = Image(file_path)
#     # img = tf.image.resize(img, (224, 224))
#     # inp = tf.keras.applications.mobilenet_v2.preprocess_input(inp)
#     # prediction = model.predict(tf.expand_dims(inp, axis=0))[0]
#     # print({class_names[i]: float(prediction[i]) for i in range(6)})
#     # return "Juda zo'r"


def detect_class(image_path):

    data = np.ndarray(shape=(1, 224, 224, 3), dtype=np.float32)

    image = Image.open(image_path).convert("RGB")

    size = (224, 224)
    image = ImageOps.fit(image, size, Image.Resampling.LANCZOS)

    image_array = np.asarray(image)

    normalized_image_array = (image_array.astype(np.float32) / 127.5) - 1

    data[0] = normalized_image_array

    prediction = model.predict(data)
    index = np.argmax(prediction)
    class_name = class_names[index]
    confidence_score = prediction[0][index]

    # print("Class:", class_name[2:], end="")
    # print("Confidence Score:", confidence_score)
    return class_name[2:]



def get_data_from_device():
    ip = "192.168.91.119/"
    date = str(time())
    havonamlik = get(ip + "humidity").content
    temperature = get(ip + "temperature").content
    tuproqnamlik = get(ip + "tuproqnamlik").content
    suvbormi = "Bor" if get(ip + "fotorezistor").content == "1" else "Yo'q"
    motoryoniqmi = "Yo'q"
    return {
        "date": date,
        "havonamlik": havonamlik,
        "temperature": temperature,
        "tuproqnamlik": tuproqnamlik,
        "suvbormi": suvbormi,
        "motoryoniqmi": motoryoniqmi   
    }
    # return{
    #     "date": str(ctime(int(time()))),
    #     "havonamlik": "40",
    #     "temperature": 30,
    #     "tuproqnamlik": 30,
    #     "suvbormi": "Ha",
    #     "motoryoniqmi": "Yo'q"   
    # }


def file_save(request, name):
    
    file = request.files[name]
    allowed_extensions = []

    if name == "picture":
        allowed_extensions = ["jpg", "jpeg", "png", "bmp"]
    else:
        allowed_extensions = ["docx", "doc", "pdf", "txt"]
    
    filename = file.filename

    if filename == "":
        return None 
    extension = filename.rsplit(".", 1)[1].lower()
    print(extension)
    if extension not in allowed_extensions:
        print("ESSIZ")
        return None
    save_filepath = "uploads/"+uuid4().hex + f".{extension}"
    file.save("static/" + save_filepath)
    return "static/" + save_filepath


@app.route("/", methods = ["POST", "GET"])
def index():
    return render_template("index.html")


@app.route("/my_device", methods = ["POST", "GET"])
def my_device():

    if request.method == "GET":
        return render_template("my_device.html")
    data = get_data_from_device()
    return data
 


@app.route("/detect_disease", methods = ["POST", "GET"])
def detect_disease():

    
    if request.method == "POST":
        file_path = file_save(request, "picture")
        if not file_path:
            return redirect("/detect_disease?error=1")
        # print(file_path)
        classname = detect_class(file_path)
        # classname = "So'glom olma"
        return render_template("detect_disease.html", classname = classname, file_path = file_path)

    
    return render_template("detect_disease.html", classname = "no")


@app.route("/login", methods = ["POST", "GET"])
def login():
    return render_template("login.html")


@app.route("/signup", methods = ["POST", "GET"])
def sign_up():
    return render_template("signup.html")


def main():
    app.run(host = "localhost", debug = True)


if __name__ == "__main__":
    main()
