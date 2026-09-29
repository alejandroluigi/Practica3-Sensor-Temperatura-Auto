import tkinter as tk
from tkinter import ttk
import serial
import threading
import time

# ==========================================
# CONFIGURACIÓN
# ==========================================

PUERTO = "COM3"
BAUDRATE = 9600


# ==========================================
# CONEXIÓN CON ARDUINO
# ==========================================

try:

    arduino = serial.Serial(
        PUERTO,
        BAUDRATE,
        timeout=1
    )

    time.sleep(2)

except Exception as e:

    arduino = None

    print("Error al conectar con Arduino:")
    print(e)


# ==========================================
# FUNCIONES
# ==========================================

def modo_sistema():

    if arduino is not None:

        arduino.write(b'S')

        etiqueta_modo.config(
            text="MODO: SISTEMA"
        )


def modo_multimetro():

    if arduino is not None:

        arduino.write(b'M')

        etiqueta_modo.config(
            text="MODO: MULTÍMETRO"
        )


def recibir_datos():

    while True:

        if arduino is not None:

            try:

                linea = arduino.readline().decode(
                    "utf-8",
                    errors="ignore"
                ).strip()


                if linea.startswith("SISTEMA"):

                    datos = linea.split(",")


                    if len(datos) == 4:

                        vout = datos[1]
                        resistencia = datos[2]
                        temperatura = datos[3]


                        ventana.after(
                            0,
                            actualizar_interfaz,
                            vout,
                            resistencia,
                            temperatura
                        )


            except Exception as e:

                print("Error:", e)

        time.sleep(0.05)


def actualizar_interfaz(
    vout,
    resistencia,
    temperatura
):

    etiqueta_vout.config(
        text=f"{vout} V"
    )

    etiqueta_resistencia.config(
        text=f"{resistencia} Ω"
    )

    etiqueta_temperatura.config(
        text=f"{temperatura} °C"
    )


def cerrar():

    if arduino is not None:

        arduino.close()

    ventana.destroy()


# ==========================================
# VENTANA
# ==========================================

ventana = tk.Tk()

ventana.title(
    "Sistema de medición Pt100"
)

ventana.geometry(
    "600x450"
)

ventana.resizable(
    False,
    False
)


# ==========================================
# TITULO
# ==========================================

titulo = tk.Label(
    ventana,
    text="SISTEMA DE MEDICIÓN PT100",
    font=("Arial", 22, "bold")
)

titulo.pack(
    pady=20
)


# ==========================================
# MODO
# ==========================================

etiqueta_modo = tk.Label(
    ventana,
    text="MODO: SISTEMA",
    font=("Arial", 16, "bold")
)

etiqueta_modo.pack(
    pady=10
)


# ==========================================
# VOUT
# ==========================================

tk.Label(
    ventana,
    text="Voltaje de salida:",
    font=("Arial", 13)
).pack()

etiqueta_vout = tk.Label(
    ventana,
    text="0.000 V",
    font=("Arial", 20)
)

etiqueta_vout.pack(
    pady=5
)


# ==========================================
# RESISTENCIA
# ==========================================

tk.Label(
    ventana,
    text="Resistencia Pt100:",
    font=("Arial", 13)
).pack()

etiqueta_resistencia = tk.Label(
    ventana,
    text="0.00 Ω",
    font=("Arial", 24, "bold")
)

etiqueta_resistencia.pack(
    pady=5
)


# ==========================================
# TEMPERATURA
# ==========================================

tk.Label(
    ventana,
    text="Temperatura:",
    font=("Arial", 13)
).pack()

etiqueta_temperatura = tk.Label(
    ventana,
    text="0.00 °C",
    font=("Arial", 30, "bold")
)

etiqueta_temperatura.pack(
    pady=10
)


# ==========================================
# BOTONES
# ==========================================

frame_botones = tk.Frame(
    ventana
)

frame_botones.pack(
    pady=20
)


boton_sistema = tk.Button(
    frame_botones,
    text="MEDIR CON SISTEMA",
    font=("Arial", 13, "bold"),
    width=20,
    command=modo_sistema
)

boton_sistema.grid(
    row=0,
    column=0,
    padx=10
)


boton_multimetro = tk.Button(
    frame_botones,
    text="MEDIR CON MULTÍMETRO",
    font=("Arial", 13, "bold"),
    width=20,
    command=modo_multimetro
)

boton_multimetro.grid(
    row=0,
    column=1,
    padx=10
)


# ==========================================
# HILO DE COMUNICACIÓN
# ==========================================

hilo = threading.Thread(
    target=recibir_datos,
    daemon=True
)

hilo.start()


# ==========================================
# CERRAR
# ==========================================

ventana.protocol(
    "WM_DELETE_WINDOW",
    cerrar
)


ventana.mainloop()
