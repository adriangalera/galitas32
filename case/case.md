# Case design

You are designing a **3D-printable enclosure for a DIY ESP32 home alarm system**. The final files will be imported into **Tinkercad** and printed on an **Artillery Genius** FDM printer.

The goal is to create a case that looks like a **commercial alarm/photo-detector**, not like a generic rectangular electronics project box.

## Overall appearance

Use the visual language of commercial wall-mounted alarm photo detectors such as the Securitas Direct/Verisure photo detector:

* Tall vertical enclosure
* Approximately **190 mm tall × 74 mm wide × 44 mm deep**
* Smooth rounded sides
* Rounded top and bottom
* Slightly convex/soft front profile
* Minimal visible fasteners
* Camera positioned in the upper portion
* Clean consumer-product appearance
* No visible ventilation grilles or exposed electronics on the front

Do not copy a commercial enclosure exactly. Use it only as design inspiration.

The case consists primarily of:

1. Front shell
2. Rear shell
3. Adjustable camera cradle
4. Modular Dupont connector inserts

No separate decorative camera bezel is required.

## Front and rear enclosure

Design the enclosure as two removable halves.

### Front shell

The front shell contains:

* Camera opening near the upper section
* Camera mounting mechanism
* Two upper hooks/clips that engage the rear shell
* Two lower bosses for M3 heat-set inserts
* Smooth exterior with no camera support structures protruding through the outer surface

A previous design had camera-support geometry intersecting the outer front wall. This must **not happen**.

All camera brackets, pivot supports, slots, bosses, etc. must remain completely behind the inner surface of the enclosure.

Only the intended camera aperture should penetrate the front.

### Rear shell

The rear shell should be mostly solid.

Do **not** create a large rectangular access opening.

It contains:

* Matching receivers/catches for the two upper front-shell clips
* Two recessed M3 screw holes near the lower portion
* The M3 screws must pass through **solid plastic**
* Modular Dupont connector bays
* Wall-mounting provisions
* Any necessary cable-management features

The front and rear closure works like this:

1. Engage the two front hooks with the corresponding rear catches.
2. Swing the front shell closed.
3. Insert two M3 screws from the rear/bottom.
4. The screws engage M3 heat-set inserts installed in the front-shell bosses.

The screw heads must not be visible from the front.

The upper clips and rear catches must actually overlap across the front/rear seam. Do not merely put decorative hooks near each other.

## Heat-set inserts

The user already owns a kit containing:

* M2
* M2.5
* M3
* M4
* M5
* M6

heat-set brass threaded inserts.

Use:

* **M3 inserts for the main enclosure closure**
* **M2.5 hardware for the adjustable camera mechanism**

The model should have sensible pilot holes for heat-set inserts but avoid assuming that nominal insert OD equals pilot-hole diameter. Leave dimensions easy to modify.

## Electronics

The main controller is an:

**ESP32-S3 WROOM N16R8 camera development board with OV2640**

Approximate PCB dimensions:

**57.1 × 28.2 mm**

Provide enough internal room for:

* The ESP32-S3 board
* GPIO/Dupont connections on the headers
* OV2640 ribbon cable
* microSD access/clearance
* USB-C connector clearance
* Internal wiring
* 5 V distribution wiring

Do not trap the ESP32 permanently in the enclosure.

Use removable supports, rails, clips, or mounting posts.

Allow some extra PCB clearance instead of designing an exact friction-fit pocket.

The case might have enough space to hold the buzzer: **SFM-27** which aprox dimension: 30x15 mm

## Adjustable OV2640 camera

The OV2640 camera must have its own internal cradle.

The camera needs to point **downward**, because the alarm will be mounted on a wall.

Required adjustment range:

**approximately 10° to 35° downward**

A good normal position is around **20° downward**.

### Camera aperture

The front camera opening must accommodate the tilted camera.

Do not use a very tight circular tunnel that clips the field of view.

Make the aperture sufficiently tall or internally chamfered/recessed to accommodate the 10°–35° tilt range.

## Dupont connector system

The user does not want to buy new external connectors.

They already have many standard Dupont wires and want to use them for quick disconnects.

Use standard **2.54 mm pitch** Dupont/header geometry.

The enclosure must contain interchangeable connector bays rather than one generic cable opening.

Create separate removable inserts for:

* 2-pin
* 3-pin
* 4-pin
* 6-pin

These should allow ordinary male 2.54 mm header pins to be mounted so female Dupont connectors can plug into them externally.

Likely assignments:

* LED strip: 3 or 4 pin
* RFID/SPI: up to 6 pin

The inserts must be mechanically retained so unplugging a Dupont cable does not push the header inside the enclosure.

Keep the system modular so the inserts can be rearranged/replaced.

## Internal wiring considerations

There should be space for an internal 5 V and GND distribution system.

Do not assume all peripheral current passes through the ESP32 board.

Allow room for:

* 5 V distribution
* Ground distribution
* MOSFET/transistor circuitry for the siren if needed
* RFID wiring
* LED wiring

## Tinkercad compatibility

The user will modify the design in **Tinkercad**.

Therefore:

* Export each meaningful component as a separate STL.
* Avoid unnecessarily complex assemblies.
* Use clean manifold geometry.
* STL scale must import correctly at **100%**.
* Keep individual components easy to position and edit.

Also provide the parametric source file, preferably:

* `alarm_case.scad`

Package everything into:

* `alarm_case_bundle.zip`

## Printability

Design for FDM printing using approximately:

* 0.4 mm nozzle
* 0.2 mm layer height
* PLA or PETG

Prefer geometry that requires minimal support.

PETG may be used for clips, so the snap features should be printable and reasonably durable.

Avoid extremely thin clip arms or fragile details.

## Geometry checks before delivering

Before exporting the final bundle, explicitly verify all of the following:

* Front STL is watertight/manifold.
* Rear STL is watertight/manifold.
* Camera cradle is watertight/manifold.
* Dupont inserts are watertight/manifold.
* No internal camera bracket protrudes through the external front surface.
* Both pivot-support sides have correctly aligned coaxial holes.
* The M2.5 pivot bolt can physically pass through both supports and the cradle.
* The locking screw and curved slot are on only one side.
* The camera cradle can actually rotate through approximately 10°–35°.
* The camera cradle does not collide with the front shell throughout its adjustment range.
* The ribbon cable has a plausible path without being pinched.
* The camera aperture does not obstruct the camera when tilted.
* Front upper hooks actually engage rear catches.
* Rear M3 holes pass through solid rear-shell material.
* M3 holes align with the corresponding heat-set insert bosses.
* Dupont bays do not interfere with the M3 screws, clips, PCB, or camera.
* The angle gauge physically fits inside the intended free space.
* All parts fit within an Artillery Genius print bed.

Do not merely generate the STLs and assume the pieces fit. Perform geometric/interference checks on the complete assembled model before delivering the files.

The priority is a **clean commercial-looking alarm enclosure that is serviceable, mechanically understandable, Tinkercad-friendly, and actually printable**, rather than a typical rectangular hobby electronics box.
