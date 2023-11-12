from keras.models import load_model
from PIL import Image, ImageOps
import numpy as np


def detect_class(image_path):
    np.set_printoptions(suppress=True)

    model = load_model("keras_model.h5", compile = False)

    class_names = open("labels.txt", "r").readlines()

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

if __name__ == "__main__":
    print(detect_class("/content/fb27b16c-4ac1-45e9-abd2-487a1a528fb0___FREC_C.Rust 0206_newGRR.JPG"))