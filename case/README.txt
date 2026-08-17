ESP32 Alarm Case - revision v4

Revision focus
--------------
This revision adds a dedicated 5 V USB power-cable entry to the lower edge of the rear shell while preserving the v3 layout:
- +5 V vertical bus on the left inner side (x = -29 mm)
- GND vertical bus on the right inner side (x = +29 mm)
- ESP32 cradle in the center
- Dupont bays in the lower section
- M3 closure screws below/around the connector zone

USB power entry
---------------
A centered, downward-facing notch is now integrated into the lower rear edge. It is intentionally open to the bottom so the cable can be laid into the rear shell during assembly/service instead of requiring the entire cable to be threaded through a closed hole.

Default entry parameters:
- usb_entry_w = 16.0 mm
- usb_entry_h = 10.0 mm
- usb_entry_clearance = 0.8 mm
- usb_entry_x = 0 mm
- usb_entry_z = -92.0 mm

Measure the actual USB plug/cable before final printing and tune these values if needed.

Strain relief
-------------
Two internal zip-tie bridges are integrated directly above the USB entry. A small cable tie can restrain the incoming power cable so an external pull is not transferred directly to soldered 5 V / GND connections.

The strain-relief bridge geometry stays inside the enclosure and does not penetrate the exterior rear wall.

Files
-----
alarm_case.scad
front_shell.stl
rear_shell.stl
rear_shell_v4.stl
camera_cradle.stl
dupont_insert_2pin.stl
dupont_insert_3pin.stl
dupont_insert_4pin.stl
dupont_insert_6pin.stl
camera_angle_gauge.stl
qa_mesh_v4.json
verification_report_v4.txt
rear_shell_v4_preview.png
rear_shell_v4_interior.png
