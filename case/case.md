# Especificación completa para agente IA — Carcasa 3D para alarma ESP32-S3

Diseña una **carcasa imprimible en 3D para un sistema de alarma doméstico DIY basado en ESP32-S3**, destinada a ser modificada posteriormente en **Tinkercad** e impresa mediante FDM en una **Artillery Genius**.

Estas instrucciones consolidan las decisiones de diseño tomadas hasta ahora. **No deben revertirse decisiones ya establecidas**, especialmente la posición inferior de los conectores, los buses verticales separados, el sistema de cámara y la entrada inferior del cable USB.

El objetivo no es simplemente producir STL visualmente correctos: el conjunto debe ser **mecánicamente montable, desmontable, imprimible y verificable**.

---

# 1. Objetivo estético

La carcasa debe parecer un **detector fotográfico de alarma comercial montado en pared**, no una caja rectangular genérica para proyectos Arduino.

Puede utilizarse como inspiración general el lenguaje visual de detectores fotográficos de sistemas como Verisure/Securitas Direct, pero **sin copiar exactamente ningún producto comercial**.

Dimensiones exteriores objetivo aproximadas:

* Alto: **190 mm**
* Ancho: **74 mm**
* Profundidad: **44 mm**

Características visuales:

* Formato vertical
* Laterales redondeados
* Parte superior redondeada
* Parte inferior redondeada
* Frontal ligeramente convexo/suavizado
* Superficies exteriores limpias
* Cámara en la zona superior
* Sin electrónica visible
* Sin rejillas de ventilación visibles en el frontal
* Sin tornillos visibles desde el frontal
* Sin estructuras mecánicas sobresaliendo por la superficie frontal

Priorizar una apariencia de **producto de consumo terminado**.

---

# 2. Componentes principales

El diseño debe incluir como mínimo:

1. `front_shell.stl`
2. `rear_shell.stl`
3. `camera_cradle.stl`
4. `dupont_insert_2pin.stl`
5. `dupont_insert_3pin.stl`
6. `dupont_insert_4pin.stl`
7. `dupont_insert_6pin.stl`

También es recomendable incluir:

8. `dupont_insert_blank.stl`
9. `camera_angle_gauge.stl`

Debe entregarse además el archivo paramétrico:

`alarm_case.scad`

---

# 3. Arquitectura frontal/trasera

La carcasa se divide en dos piezas principales:

* Frontal desmontable
* Trasera fija

La arquitectura de mantenimiento debe permitir que **la trasera permanezca montada en la pared al abrir la alarma**.

La mayor parte de la electrónica debe permanecer en la trasera:

* ESP32-S3
* Buses +5 V y GND
* Conectores Dupont
* Cableado de periféricos
* Electrónica auxiliar
* Driver/MOSFET de sirena

El frontal debe contener principalmente:

* Cámara
* Cradle de cámara
* Mecanismo de ajuste

Idealmente, el único enlace eléctrico que debe permanecer entre frontal y trasera al abrir la caja es el **ribbon de la OV2640**.

---

# 4. Sistema de cierre

La carcasa debe cerrarse mediante:

* Dos ganchos superiores
* Dos tornillos M3 inferiores

Procedimiento:

1. Introducir los dos ganchos superiores del frontal en los catches de la trasera.
2. Girar/bascular el frontal hacia la posición cerrada.
3. Introducir dos tornillos M3 desde la parte trasera/inferior.
4. Los tornillos roscan en insertos térmicos M3 instalados en bosses del frontal.

Los ganchos deben **solaparse físicamente** con los catches.

No crear ganchos puramente decorativos que solamente queden cerca de los receptores.

Los tornillos M3 deben:

* Atravesar plástico sólido de la trasera
* Estar alineados con los bosses del frontal
* Tener las cabezas recesadas
* No ser visibles desde el frontal

---

# 5. Insertos térmicos

El usuario dispone de insertos térmicos:

* M2
* M2.5
* M3
* M4
* M5
* M6

Usar:

* **M3 para el cierre principal**
* **M2.5 para el mecanismo ajustable de cámara**

No asumir que el diámetro nominal del inserto es el diámetro correcto del agujero impreso.

Parametrizar explícitamente:

* `m3_clearance`
* `m3_insert_pilot`
* `m25_clearance`
* `m25_insert_pilot`, si se utiliza inserto en el mecanismo

---

# 6. Frontal

El frontal debe incluir:

* Apertura de cámara
* Dos ganchos superiores
* Dos bosses inferiores para M3
* Soportes internos de cámara

La superficie exterior debe permanecer limpia.

## Restricción crítica

**Ninguna geometría de soporte de cámara puede atravesar la superficie exterior frontal.**

Esto incluye:

* Pivot supports
* Bosses
* Brackets
* Locking slot
* Refuerzos
* Orejetas
* Tornillos

Todos deben permanecer detrás de la superficie interior.

La única penetración relacionada con la cámara será la **apertura óptica**.

---

# 7. Cámara OV2640

La cámara es una **OV2640** conectada mediante ribbon al ESP32-S3.

Debe apuntar hacia abajo porque el dispositivo estará instalado verticalmente en una pared.

Rango de ajuste:

* **10° downward mínimo**
* **20° downward posición nominal**
* **35° downward máximo**

El cradle debe poder detenerse en cualquier posición razonable dentro de ese intervalo mediante un tornillo de bloqueo.

---

# 8. Pivote de cámara

Utilizar un eje horizontal.

Debe existir:

* Soporte fijo izquierdo
* Cradle
* Soporte fijo derecho

Un tornillo/eje **M2.5** debe poder atravesar físicamente:

```text
soporte izquierdo
       ↓
     cradle
       ↓
soporte derecho
```

Los tres agujeros deben ser:

* Coaxiales
* Correctamente alineados
* Con holgura adecuada

No diseñar dos pivotes independientes que no compartan exactamente el mismo eje.

---

# 9. Bloqueo angular

El sistema de bloqueo debe existir en **un solo lateral**.

Utilizar:

* Slot curvo
* Tornillo M2.5

El lateral opuesto debe ser únicamente pivote.

No crear dos slots de bloqueo.

El slot debe:

* Tener el mismo centro geométrico que el pivote
* Seguir correctamente la dirección real de rotación
* Cubrir aproximadamente 10°–35°
* Tener suficiente material alrededor

---

# 10. Apertura óptica

No utilizar un túnel circular estrecho.

La abertura debe admitir el campo de visión de la cámara en todo el rango de movimiento.

Puede ser:

* Verticalmente alargada
* Ovalada
* Chamfered internamente
* Recesada

Debe comprobarse como mínimo a:

* 10°
* 15°
* 20°
* 25°
* 30°
* 35°

---

# 11. Ribbon OV2640

Debe existir una ruta realista para el ribbon.

No debe:

* Pasar por el eje del pivote
* Ser pinzado al cerrar la carcasa
* Rozar aristas afiladas
* Interferir con el tornillo de bloqueo
* Quedar excesivamente doblado
* Impedir el movimiento del cradle

Dejar suficiente volumen detrás/debajo de la cámara para formar una curva suave.

---

# 12. ESP32-S3

Controlador:

**ESP32-S3 WROOM N16R8 camera development board con OV2640**

Dimensiones aproximadas del PCB:

**57.1 × 28.2 mm**

No diseñar un pocket de fricción exacta.

Añadir holgura para:

* PCB
* Pines
* Headers
* Dupont
* USB-C
* microSD
* Ribbon
* Cableado

---

# 13. Cradle del ESP32

La placa debe estar montada en la **trasera**.

Utilizar un sistema desmontable mediante:

* Rails
* Clips
* Posts
* Retenedores equivalentes

El cradle central utilizado hasta ahora es conceptualmente válido.

La placa debe poder retirarse para mantenimiento.

No atraparla permanentemente entre estructuras impresas.

---

# 14. Acceso USB-C y microSD del ESP32

Aunque exista una entrada independiente para alimentación general, debe mantenerse espacio para acceder al:

* USB-C del ESP32
* microSD

Verificar físicamente que:

* Puede conectarse un cable USB-C
* Los buses no bloquean el conector
* Los Dupont no bloquean el conector
* La placa puede retirarse
* La microSD tiene espacio razonable para inserción/extracción

---

# 15. Organización general de la trasera

La distribución preferida de arriba hacia abajo es:

```text
┌─────────────────────────────┐
│     CATCHES / WALL MOUNT    │
│                             │
│        ZONA CÁMARA          │
│                             │
│ +5V       ESP32       GND   │
│  ║          │          ║    │
│  ║          │          ║    │
│  ║          │          ║    │
│  ║                     ║    │
│                             │
│    ZONA AUXILIAR/BUZZER     │
│                             │
│ [BAY] [BAY] [BAY] [BAY]    │
│                             │
│ M3      USB POWER      M3   │
└────────────┬────────────────┘
             │
             ▼
          cable USB
```

La representación es conceptual y puede ajustarse para evitar interferencias.

---

# 16. Eliminar elementos innecesarios

No volver a introducir las protuberancias extrañas presentes en versiones iniciales.

Evitar:

* Cable loops laterales voluminosos
* Shelves arbitrarios
* Bosses sin función
* Soportes decorativos
* Estructuras laterales innecesarias
* Grandes bandejas dedicadas a un único componente

Mantener el interior limpio.

---

# 17. Alimentación general

El sistema utilizará **5 V**.

No asumir que toda la corriente de los periféricos debe circular a través del ESP32.

Arquitectura:

```text
              ENTRADA 5 V
                  │
          ┌───────┴───────┐
          │               │
       BUS +5V         BUS GND
          │               │
    ┌─────┼─────┐    ┌────┼─────┐
    │     │     │    │    │     │
 ESP32  RFID   LED  ESP32 RFID  LED
          │               │
       DRIVER          SIRENA
```

El ESP32 es una rama de alimentación más.

---

# 18. Sirena

La corriente de una sirena no debe ser suministrada directamente por un GPIO.

Utilizar:

* MOSFET
* Transistor
* Driver equivalente

El GPIO solamente proporciona la señal de control.

Debe existir espacio interior para este pequeño circuito.

---

# 19. Buses de alimentación

La trasera debe incorporar soportes para dos buses independientes:

* `+5V`
* `GND`

Los buses deben estar dispuestos **verticalmente**.

Configuración consolidada:

* `+5V` en el lateral interior izquierdo
* `GND` en el lateral interior derecho

No colocar ambos buses juntos.

---

# 20. Separación física de buses

Separarlos tanto como sea razonable dentro de la carcasa.

Objetivo aproximado:

**55–60 mm entre ejes**, si la geometría lo permite.

La razón principal es mecánica y de seguridad:

* Evitar puentes de estaño
* Evitar cortos accidentales
* Evitar tocar simultáneamente ambos buses con herramientas
* Facilitar soldadura y mantenimiento

---

# 21. Orientación vertical

No colocar los buses horizontalmente.

Aprovechar la altura de aproximadamente 190 mm.

Conceptualmente:

```text
+5V                          GND
 ║                            ║
 ╠── ESP32 5V        ESP32 ───╣
 ║                            ║
 ╠── RFID 5V          RFID ───╣
 ║                            ║
 ╠── LED 5V            LED ───╣
 ║                            ║
 ╠── DRIVER          DRIVER ───╣
 ║                            ║
 ╚── etc.              etc. ───╣
```

Las señales pueden circular principalmente por la zona central.

---

# 22. Material de los buses

Los buses pueden realizarse mediante:

* Hilo de cobre rígido
* Barra fina de cobre
* Conductor rígido equivalente

No aplicar estaño directamente sobre la pared de PLA/PETG.

El plástico solamente debe proporcionar **soporte mecánico y aislamiento**.

---

# 23. Clips para buses

Crear pequeños clips integrados en la trasera.

Características:

* Aproximadamente 4–6 clips por bus
* Distribuidos verticalmente
* Separación aproximada 20–30 mm
* Apertura hacia el **centro de la carcasa**

La apertura hacia el centro permite colocar el conductor lateralmente:

```text
PARED             INTERIOR

│   ┌──
│   │ ●  ← cobre
│   └──
│
```

No utilizar una sucesión de agujeros cerrados que obligue a introducir el conductor longitudinalmente.

---

# 24. Parámetros del bus

Parametrizar:

* `bus_wire_diameter`
* `bus_wire_clearance`
* `bus_clip_spacing`
* `bus_left_position`
* `bus_right_position`
* `bus_start_height`
* `bus_end_height`

Valor inicial orientativo:

`bus_wire_diameter = 1.6 mm`

El usuario deberá poder cambiarlo después de medir el conductor real.

---

# 25. Longitud de los buses

No extenderlos hasta la parte superior de la carcasa si no es necesario.

Deben:

* Comenzar debajo de la zona de cámara
* Recorrer la zona útil de electrónica
* Terminar antes de la zona inferior donde puedan interferir con Dupont, USB o M3

---

# 26. Kit Dupont disponible

El usuario dispone de un kit estándar Dupont de **2.54 mm** que permite fabricar conectores mediante crimpado.

Dispone de:

* Terminales macho
* Terminales hembra
* Carcasas multipin

Por tanto, aprovechar **conectores Dupont eléctricos reales**.

---

# 27. Arquitectura de conectores externos

La solución preferida es:

### Periférico

Cable terminado en **conector Dupont macho multipin**.

### Carcasa

Conector **Dupont hembra multipin** sujeto mecánicamente mediante un insert impreso.

Conceptualmente:

```text
EXTERIOR                   INTERIOR

Periférico
    │
┌───────────┐
│ DUPONT    │
│ MACHO     │
└──●─●─●────┘
   ↓ ↓ ↓
┌──○─○─○────┐
│ DUPONT    │
│ HEMBRA    │
└───────────┘
█████████████ ← insert/retención impresa
    │ │ │
    │ │ └── señal
    │ └──── GND
    └────── +5V
```

---

# 28. Función del insert Dupont

El plástico impreso **no sustituye el conector Dupont**.

Su función es exclusivamente:

* Sujetar la carcasa Dupont hembra
* Evitar que se introduzca hacia dentro
* Evitar que salga hacia fuera
* Resistir las fuerzas de conexión/desconexión

Debe poder retirarse durante mantenimiento.

---

# 29. Inserts Dupont requeridos

Crear:

* 2P
* 3P
* 4P
* 6P

También es recomendable:

* Insert ciego

---

# 30. Bahías modulares

Preferir varias bahías con **geometría externa común**.

Cada bahía debería poder recibir distintos inserts.

Por ejemplo:

```text
[BAY 1] [BAY 2] [BAY 3] [BAY 4]
```

y cada una podría utilizar:

* 2P
* 3P
* 4P
* 6P
* Blank

si la solución mecánica lo permite.

---

# 31. Ubicación de Dupont

Esta decisión es obligatoria:

**Todos los conectores Dupont externos deben encontrarse en la parte inferior de la trasera.**

No colocarlos:

* Junto al ESP32
* En mitad de la carcasa
* En la zona superior
* Dispersos por los laterales

Los cables externos deben poder salir naturalmente hacia abajo.

---

# 32. Cableado de Dupont

Ejemplo RFID 6P:

```text
+5V  ─────────► BUS +5V
GND  ─────────► BUS GND
SCK  ─────────► ESP32
MOSI ─────────► ESP32
MISO ─────────► ESP32
CS   ─────────► ESP32
```

Los conductores de alimentación se dirigen a los laterales.

Las señales pueden subir por la zona central.

---

# 33. Entrada USB de alimentación

Debe existir una abertura específica para introducir desde el exterior el **cable USB utilizado como alimentación principal de 5 V**.

Esta entrada es independiente de:

* Los conectores Dupont
* El USB-C de programación del ESP32

---

# 34. Posición del USB de alimentación

Situar la entrada en la **parte inferior**, preferiblemente centrada aproximadamente entre los dos tornillos M3 cuando la geometría lo permita.

Conceptualmente:

```text
│ [DUPONT] [DUPONT] [DUPONT] │
│                             │
│ M3      USB POWER       M3  │
└────────────┬────────────────┘
             │
             ▼
           cable
```

El cable debe salir naturalmente hacia abajo.

---

# 35. Apertura USB

La abertura debe permitir introducir **la cabeza completa del conector USB**, no únicamente el diámetro del cable.

Valor inicial orientativo utilizado:

* Ancho: aproximadamente **16 mm**
* Alto: aproximadamente **10 mm**
* Holgura adicional: aproximadamente **0.8 mm**

Estos valores **no son definitivos**.

Deben quedar parametrizados porque el usuario medirá posteriormente su cable real.

Crear:

* `usb_cable_hole_width`
* `usb_cable_hole_height`
* `usb_cable_clearance`
* `usb_cable_hole_position`

---

# 36. Forma de la entrada USB

Preferir una abertura/muesca accesible desde la parte inferior en lugar de un simple agujero circular.

La sustitución del cable debe ser sencilla.

Evitar obligar al usuario a:

* Cortar el cable
* Desoldarlo para retirarlo
* Desmontar innecesariamente toda la electrónica

---

# 37. Strain relief USB

Incorporar alivio de tensión interior.

Solución preferida:

**dos pequeños puntos/loops/posts para una brida**.

La brida debe sujetar el cable después de entrar en la carcasa.

Así:

```text
exterior
   │
   │ cable
   ▼
[entrada USB]
   │
   ├────(brida)────┐
   │               │
   └───────────────┘
         │
         ├── +5V → bus +5V
         └── GND → bus GND
```

Un tirón exterior no debe transmitirse directamente a las soldaduras.

---

# 38. Entrada USB vs USB-C ESP32

Distinguir claramente:

### Entrada USB principal

Proporciona alimentación general de 5 V al sistema.

### USB-C ESP32

Se utiliza para:

* Programación
* Depuración
* Alimentación alternativa cuando proceda

El diseño no debe confundir ambos.

---

# 39. Buzzer SFM-27

Debe reservarse espacio para un buzzer aproximadamente:

**SFM-27, ~30 × 15 mm**

No utilizar únicamente sus dimensiones nominales.

Reservar un volumen aproximado de al menos:

**35 × 20 × 20 mm**

para incluir:

* Cuerpo
* Terminales
* Soldaduras
* Cables
* Holgura de montaje

---

# 40. Posición del buzzer

Preferir una zona libre:

* Entre cámara y ESP32, si existe suficiente profundidad
* O en otra zona interna libre que no interfiera con los buses

No bloquear:

* Cámara
* Ribbon
* ESP32
* USB-C
* microSD
* Dupont
* Buses

---

# 41. Retención del buzzer

No volver a crear una gran bandeja/shelf como en diseños anteriores.

Si se proporciona retención específica, utilizar algo mínimo:

* 2–3 pequeños clips
* Topes
* Clip + tope
* Sistema equivalente

El buzzer debe ser **desmontable**.

No pegarlo permanentemente ni atraparlo entre las dos mitades.

---

# 42. Validación específica del buzzer

Antes de finalizar, comprobar con un volumen de referencia de al menos:

**35 × 20 × 20 mm**

que:

* Cabe dentro de la carcasa cerrada
* No toca el frontal
* No toca la cámara
* No limita el ajuste 10°–35°
* No pinza el ribbon
* No interfiere con ESP32
* No bloquea USB-C
* No bloquea microSD
* No toca los buses
* Sus cables tienen una ruta plausible

No declarar simplemente que “hay espacio” basándose en observación visual.

---

# 43. Montaje en pared

Usar un sistema sencillo.

Preferido:

* Keyhole superior
* Agujero secundario pequeño antirotación

Evitar grandes brackets externos.

Debe ser compatible con mantener la trasera instalada mientras se abre el frontal.

---

# 44. Printabilidad

Diseñar para aproximadamente:

* Boquilla: **0.4 mm**
* Layer height: **0.2 mm**
* Material: PLA o PETG

PETG puede utilizarse especialmente para:

* Ganchos
* Clips
* Retenedores Dupont
* Clips de buses

Evitar:

* Paredes extremadamente finas
* Clips frágiles
* Overhangs innecesarios
* Soportes de impresión excesivos
* Geometría interna imposible de imprimir

---

# 45. Compatibilidad Tinkercad

Todos los STL deben:

* Utilizar mm
* Importar a escala **100 %**
* Tener geometría limpia
* Ser manifold
* Ser watertight
* Evitar polygon counts absurdamente altos
* Poder posicionarse fácilmente
* Estar separados por componente funcional

No crear una única STL con todo el conjunto fusionado.

---

# 46. Parametrización OpenSCAD

Entregar:

`alarm_case.scad`

Los parámetros principales deben estar agrupados al comienzo del archivo.

Como mínimo:

```text
case_height
case_width
case_depth
wall_thickness

m3_clearance
m3_insert_pilot

m25_clearance
m25_insert_pilot

camera_min_angle
camera_nominal_angle
camera_max_angle

camera_pivot_x
camera_pivot_y
camera_pivot_z

camera_aperture_width
camera_aperture_height

pcb_width
pcb_height
pcb_clearance

dupont_pitch
dupont_fit_clearance
dupont_bay_width
dupont_bay_height

bus_wire_diameter
bus_wire_clearance
bus_clip_spacing
bus_left_position
bus_right_position
bus_start_height
bus_end_height

usb_cable_hole_width
usb_cable_hole_height
usb_cable_clearance
usb_cable_hole_position

buzzer_width
buzzer_height
buzzer_depth
buzzer_clearance

clip_clearance
```

No esconder dimensiones críticas dentro de módulos difíciles de encontrar.

---

# 47. Validación de la cámara

Comprobar el conjunto ensamblado a:

* 10°
* 15°
* 20°
* 25°
* 30°
* 35°

En cada posición verificar:

* Cradle vs frontal
* Cámara vs frontal
* Apertura óptica
* Ribbon
* Tornillo de bloqueo
* Buzzer
* Otros elementos internos

---

# 48. Validación del pivote

Comprobar explícitamente:

* Coaxialidad
* Diámetro de paso
* M2.5 atravesando ambos soportes
* M2.5 atravesando cradle
* Ausencia de plástico bloqueando el eje

No inferirlo solamente de las coordenadas CAD.

---

# 49. Validación del cierre

Comprobar:

* Enganche físico de ambos hooks
* Profundidad de solape
* Ausencia de interferencia de shells
* Alineación de ambos M3
* Agujeros M3 atravesando material sólido
* Bosses correctamente posicionados
* Acceso de herramienta a las cabezas

---

# 50. Validación Dupont

Comprobar:

* Inserts caben en sus bays
* No pueden empujarse hacia dentro al conectar
* No salen al desconectar
* Hay espacio para el conector macho externo
* El cable puede salir hacia abajo
* No interfieren con M3
* No interfieren con USB power
* No bloquean PCB
* No bloquean buses

---

# 51. Validación de buses

Comprobar:

* Bus +5 V cabe
* Bus GND cabe
* Separación física suficiente
* Clips imprimibles
* Conductor insertable lateralmente
* Conductor extraíble
* Acceso con soldador razonable
* No interferencia con ESP32
* No interferencia con buzzer
* No interferencia con Dupont
* No interferencia con USB-C
* No interferencia con microSD
* No interferencia con cámara

---

# 52. Validación USB power

Comprobar:

* La cabeza del conector objetivo atraviesa la abertura
* El cable sale hacia abajo
* No existe radio de curvatura excesivamente pequeño
* Existe espacio para la brida
* La brida puede instalarse
* El cable no queda pinzado por el frontal
* No debilita excesivamente la zona de M3
* Puede sustituirse
* No interfiere con Dupont

---

# 53. Validación de malla

Para cada STL comprobar:

* Watertight
* Manifold
* Winding consistente
* Ausencia de caras degeneradas relevantes
* Ausencia de geometrías flotantes
* Una sola pieza conectada cuando corresponda

Especialmente:

* Front
* Rear
* Camera cradle
* Cada Dupont insert
* Blank insert
* Gauge

---

# 54. Cama de impresión

Comprobar que cada componente individual cabe en la cama de una **Artillery Genius**.

No asumirlo únicamente por las dimensiones nominales exteriores.

Considerar orientación real de impresión.

---

# 55. Informe de validación

Entregar un:

`validation_report.txt`

o equivalente.

No escribir únicamente:

`Validation passed`

Debe detallar qué se comprobó.

Ejemplo:

```text
Rear shell:
- Watertight: PASS
- Manifold: PASS
- Connected components: 1
- USB opening clearance: PASS
- Bus clips: PASS

Camera:
- 10 deg: PASS
- 15 deg: PASS
- 20 deg: PASS
- 25 deg: PASS
- 30 deg: PASS
- 35 deg: PASS

Buzzer envelope 35x20x20:
- Front collision: PASS
- Camera collision: PASS
- PCB collision: PASS
...
```

---

# 56. README

Incluir un `README.md` explicando:

* Qué archivo es cada pieza
* Orientación recomendada de impresión
* Hardware necesario
* M3 utilizados
* M2.5 utilizados
* Cómo instalar insertos térmicos
* Cómo montar la cámara
* Cómo montar los Dupont
* Cómo instalar los buses
* Cómo sujetar el cable USB
* Cómo montar el buzzer
* Orden de montaje de la carcasa

---

# 57. Bundle final

Empaquetar todo en:

`alarm_case_bundle.zip`

Debe contener como mínimo:

```text
alarm_case.scad

front_shell.stl
rear_shell.stl
camera_cradle.stl

dupont_insert_2pin.stl
dupont_insert_3pin.stl
dupont_insert_4pin.stl
dupont_insert_6pin.stl

README.md
validation_report.txt
```

Opcional/recomendado:

```text
dupont_insert_blank.stl
camera_angle_gauge.stl
```

---

# 58. Decisiones consolidadas que NO deben revertirse

Las siguientes decisiones proceden de iteraciones anteriores y deben considerarse requisitos:

* Aspecto de detector de alarma comercial
* Aproximadamente 190 × 74 × 44 mm
* Dos shells
* Frontal limpio
* Cámara arriba
* Cámara ajustable 10°–35°
* Posición nominal ~20°
* M2.5 para cámara
* Un único locking slot lateral
* Pivot holes coaxiales
* M3 para cierre de carcasa
* Dos hooks superiores funcionales
* Dos M3 inferiores
* ESP32 desmontable
* ESP32 en la trasera
* Sin gran abertura rectangular trasera
* Sin extrañas protuberancias laterales
* Sin cable loops voluminosos
* Dupont agrupados abajo
* Dupont eléctricos reales del kit
* Inserts impresos únicamente como soporte mecánico
* Buses de cobre independientes
* Bus +5 V vertical en un lateral
* Bus GND vertical en el lateral contrario
* Máxima separación práctica entre buses
* Clips de bus abiertos hacia el centro
* Alimentación de periféricos directamente desde los buses cuando corresponda
* No obligar a que toda la corriente atraviese el ESP32
* Entrada independiente de cable USB de alimentación
* Entrada USB en la zona inferior
* Strain relief interno mediante brida o equivalente
* Mantener acceso al USB-C del ESP32
* Reservar espacio real para SFM-27
* No utilizar una gran bandeja dedicada al buzzer
* Buzzer desmontable
* Reservar aproximadamente 35 × 20 × 20 mm para buzzer + conexiones
* Validar todas las piezas antes de entregar

---

# Prioridad final

El resultado debe ser una carcasa que tenga sentido tanto **cerrada como abierta**.

Cerrada debe parecer un producto comercial.

Abierta debe mostrar una arquitectura ordenada y fácil de mantener:

```text
          CÁMARA

+5V        ESP32         GND
 ║           │            ║
 ║       señales          ║
 ║                        ║

      BUZZER / DRIVER

   DUPONT MODULARES

M3      USB POWER       M3
```

No sacrificar la capacidad real de montaje por conseguir una geometría visualmente atractiva.

**Antes de entregar cualquier STL, verificar el ensamblaje completo y las interferencias reales entre componentes.**
