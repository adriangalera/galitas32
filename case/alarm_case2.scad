// DIY ESP32 Alarm / Photo-Detector Enclosure
// Parametric OpenSCAD source, dimensions in mm.
// Designed for 0.4 mm nozzle / 0.2 mm layers, PLA or PETG.
$fn = 48;

PART = "exploded"; // front, rear, wall_mount, pcb2, assembly, wall_assembly, exploded
// Camera stays on the ESP32-S3-CAM board, facing -Y (USB connectors downward).
// No independent camera tilt: the original short ribbon stays in its natural position.
// The front cover must not pull, clamp or carry the camera when removed.
// Use the board's existing camera retention; this cover does not retain a loose sensor.
// Photo-based optical alignment is provisional: check against the real board before printing.

// ---------------- User-tunable parameters ----------------
case_w = 74;
case_d = 44;
case_h = 190;
corner_r = 11;
wall = 2.4;
// Shared rail dimensions: the rear rim follows the front edge of the rails.
pcb_rail_y = 9.5;
pcb_rail_depth = 25;
seam_y = min(0, pcb_rail_y - pcb_rail_depth/2); // -3 mm; rear half depth = 25 mm

// PCB reference matches the existing 60 mm rail channels, not a measured board.
pcb_reference_w = 28;              // existing project value
pcb_reference_h = 60;              // provisional occupied channel height
pcb_reference_z = -3;
pcb_slot_y_offset = -8.5;
pcb_reference_y = pcb_rail_y + pcb_slot_y_offset;

// Lens is over the upper portion of the PCB, as in the supplied photograph.
// Estimated offset; change this value if the lens is higher/lower on your board.
camera_from_pcb_top = 14;
camera_x = 0;
camera_z = pcb_reference_z + pcb_reference_h/2 - camera_from_pcb_top;
// Broad, shallow window tolerates alignment uncertainty and lens setback.
// Actual field of view still depends on the lens; no optical specification is assumed.
camera_window_w = 40;
camera_window_h = 40;
camera_window_r = 4;
show_electronics = true;            // assembly preview only, never exported to STL
camera_preview_projection = 7;    // illustrative projection from PCB, not measured

// ---------------- Fixed downward wall mount ----------------
wall_tilt = 20;                   // degrees downward; change and reprint (10..35)
screw_above_frame = 85;           // measured vertical screw-to-frame-top distance
door_vertical_clearance = 85;     // frame top to door collision limit
door_safety_clearance = 5;        // minimum reserved vertical gap above that limit
frame_projection = 14;            // ASSUMED protrusion from wall; user described frame width
frame_clearance = 4;              // allowance around the frame
// Conservative bound keeps the complete case clear of the frame below the screw.
wall_bottom_clearance = (frame_projection+frame_clearance)/cos(wall_tilt)+1;
mount_rib_x = 30;
mount_rib_t = 4;
mount_z_max = 78;
mount_plate_t = 4;
mount_screw_shank_d = 4.5;         // clearance for a nominal 4 mm wall screw
mount_screw_head_d = 9;            // entry clearance; head must be wider than shank slot
mount_head_space = 4;             // free depth behind retaining plate
wall_plane_y = case_d/2 + case_h/2*tan(wall_tilt) + wall_bottom_clearance;
// Position screw seat from the conservative bounding box of the tilted case.
// This automatically raises the case enough to clear the door with margin.
case_bottom_below_wall_origin = case_h/2*cos(wall_tilt)
    + case_d/2*sin(wall_tilt) + wall_plane_y*sin(wall_tilt);
mount_seat_z = screw_above_frame + door_vertical_clearance
    - door_safety_clearance - case_bottom_below_wall_origin;
mount_entry_z = mount_seat_z-13;   // entry below seat; lower case to lock
mount_plate_z = mount_seat_z-3;
frame_top_z = mount_seat_z-screw_above_frame; // wall-frame coordinates
mount_pad_h = 12;
mount_pad_z = frame_top_z+frame_clearance+mount_pad_h/2+1;
mount_z_min = (mount_pad_z-8)*cos(wall_tilt)-10;

// Wall plane in enclosure coordinates: y = wall_plane_y + z*tan(wall_tilt).
// Thick end at TOP: rotation +wall_tilt about X points the camera DOWN.

// Mounting: insert screw head in the LOWER round opening, then lower the case.
// Start with about 4.5 mm between wall and underside of screw head; adjust to fit.
// The two lower pads may receive thin non-slip pads of equal thickness.
// Mount prints separately; verify slicer support needs for the selected orientation.

// ---------------- Separate mount / PLA weld joints ----------------
joint_plane_y = case_d/2+4;        // mating plane; installed mount position unchanged
joint_zs = [-20,60];              // four pads: two per side
joint_w = 10;
joint_h = 20;
joint_bevel = 0.7;                // paired bevels form a perimeter welding groove
joint_clearance = 0.25;           // socket clearance per side
joint_pin_depth = 2.5;
joint_screw_d = 3.4;              // two optional M3 through-bolts, upper pads only
// Assemble dry: four rear pins into four blind mount sockets, faces seated.
// Tack opposite corners, then fill exposed perimeter grooves with matching PLA.
// Do not melt locating pins before alignment; allow the complete joint to cool.
// Optional upper through-bolts use nuts inside the empty rear shell.
// Select wall_mount for a separate part placed flat on its joint faces at Z=0.

m3_screw_clearance = 3.4;
m3_insert_pilot_d = 4.2;   // intentionally easy to tune for your actual heat-set insert
m3_boss_d = 9.5;
m3_z = -71;
m3_x = 20;

bay_w = 18;
bay_h = 10;
bay_cut_w_2pin = 5.50;
bay_cut_w_3pin = 8;
bay_cut_w_5pin = 16;
bay_cut_w_8pin = 21;
bay_cut_h = 3;
bay_z = -57;   // moved to lower rear section, above closure screws
bay_xs = [-22, 0, 22];
header_pitch = 2.54;
header_hole = 1.25;

// Vertical 5 V / GND bus supports on rear shell
bus_x = 29.0;             // buses are separated across almost the full enclosure width
bus_y = 15.7;             // wire center, behind PCB and clear of the shell seam
bus_z_min = -41;          // stops above Dupont connector zone
bus_z_max = 30;           // stops below camera mechanism
bus_wire_d = 1.6;         // tune for the copper wire / rod actually used
bus_wire_clearance = 0.45;
bus_clip_zs = [-38,-20,-2,16,28];

// Main 5 V USB power cable entry, centered at the bottom rear edge.
// Sized to pass a typical USB plug; tune to the actual cable before printing.
usb_entry_w = 16.0;
usb_entry_h = 10.0;
usb_entry_clearance = 0.8;
usb_entry_x = 0;
usb_entry_z = -92.0;
usb_strain_relief_z = -81.5;
usb_strain_relief_x = 7.5;
usb_zip_tie_slot_w = 3.2;
usb_zip_tie_slot_h = 7.0;

// ---------------- Utility geometry ----------------
module rounded_profile_2d(w,h,r) {
    offset(r=r) square([w-2*r,h-2*r],center=true);
}

module rounded_prism_y(w,d,h,r) {
    rotate([90,0,0]) linear_extrude(height=d,center=true,convexity=10) rounded_profile_2d(w,h,r);
}

module soft_body(w,d,h,r) {
    // Slightly softened/convex consumer-product profile with tapered front/rear edges.
    hull() {
        translate([0,-d/2+0.7,0]) rounded_prism_y(w-1.6,1.4,h-1.6,max(2,r-0.8));
        translate([0,-d/2+3.0,0]) rounded_prism_y(w,1.2,h,r);
        translate([0, d/2-3.0,0]) rounded_prism_y(w,1.2,h,r);
        translate([0, d/2-0.7,0]) rounded_prism_y(w-1.2,1.4,h-1.2,max(2,r-0.6));
    }
}

module hole_x(d=3, len=10) { rotate([0,90,0]) cylinder(d=d, h=len, center=true); }
module hole_y(d=3, len=10) { rotate([90,0,0]) cylinder(d=d, h=len, center=true); }

module shell_skin() {
    difference() {
        soft_body(case_w,case_d,case_h,corner_r);
        soft_body(case_w-2*wall, case_d-2*wall, case_h-2*wall, max(2,corner_r-wall));
    }
}

module front_skin() {
    intersection() {
        shell_skin();
        translate([0,(seam_y-case_d/2-2)/2,0])
            cube([case_w+4,seam_y+case_d/2+2,case_h+4],center=true);
    }
}

module rear_skin() {
    intersection() {
        shell_skin();
        translate([0,(seam_y+case_d/2+2)/2,0])
            cube([case_w+4,case_d/2+2-seam_y,case_h+4],center=true);
    }
}

// ---------------- Front shell internal mechanics ----------------
module camera_aperture_cut() {
    // Through the front wall only; no internal tunnel or separate camera cradle.
    translate([camera_x,-case_d/2+0.5,camera_z])
        rounded_prism_y(camera_window_w,2*wall+4,camera_window_h,camera_window_r);
}

module front_m3_bosses() {
    for (x=[-m3_x,m3_x])
        translate([x,-10.5,m3_z])
        difference() {
            rotate([90,0,0]) cylinder(d=m3_boss_d,h=20.4,center=true);
            // blind pilot from rear-facing end; leaves solid plastic toward front exterior
            translate([0,3.6,0]) rotate([90,0,0]) cylinder(d=m3_insert_pilot_d,h=14.8,center=true);
        }
}

module front_upper_hooks() {
    // Two real seam-crossing hooks. Front half extends through Y=seam_y into rear catch volume.
    for (x=[-20,20]) {
        union() {
            translate([x,seam_y-3.0,76]) cube([8,6,4],center=true);
            translate([x,seam_y+1.25,78.2]) cube([8,2.8,4.4],center=true); // barb across seam
            hull() {
                translate([x,-18.6,74]) cube([8,3.0,4],center=true);
                translate([x,seam_y-3.0,76]) cube([8,2,4],center=true);
            }
        }
    }
}

module front_shell() {
    difference() {
        union() {
            front_skin();
            front_m3_bosses();
            front_upper_hooks();
        }
        camera_aperture_cut();
    }
}

// ---------------- Rear shell mechanics ----------------
module rear_catches() {
    // Catch roof + side walls overlap the front hook barbs across the seam.
    for (x=[-20,20])
        difference() {
            union() {
                translate([x,seam_y+3.1,80.6]) cube([11,6.2,3.4],center=true);
                translate([x-4.8,seam_y+3.1,77.8]) cube([1.8,6.2,7.5],center=true);
                translate([x+4.8,seam_y+3.1,77.8]) cube([1.8,6.2,7.5],center=true);
            }
            translate([x,seam_y+1.2,78.1]) cube([9.4,4.0,4.8],center=true); // receiving pocket
        }
    // rear-wall bridges keep catches integral while leaving hook pocket open
    for (x=[-20,20]) hull() {
        translate([x,seam_y+5.5,82.0]) cube([8,2,2.4],center=true);
        translate([x,19.0,82.0]) cube([8,2.8,2.4],center=true);
    }
}

module rear_m3_pads() {
    for (x=[-m3_x,m3_x])
        translate([x,10.6,m3_z]) rotate([90,0,0]) cylinder(d=11.5,h=21.0,center=true);
}

module rear_m3_holes() {
    for (x=[-m3_x,m3_x]) {
        translate([x,10.5,m3_z]) rotate([90,0,0]) cylinder(d=m3_screw_clearance,h=25,center=true);
        // recessed screw head from rear face, not visible from front
        translate([x,19.0,m3_z]) rotate([90,0,0]) cylinder(d=6.7,h=7.0,center=true);
    }
}

// Local wall frame: X horizontal, Y toward wall, Z vertical when installed.
module wall_frame() {
    translate([0,wall_plane_y,0]) rotate([-wall_tilt,0,0]) children();
}

module wall_mount_rib(x) {
    // Side webs attach to rear skin and stop 2 mm short of the wall contact plane.
    translate([x-mount_rib_t/2,0,0])
        rotate([90,0,90]) linear_extrude(height=mount_rib_t)
            polygon(points=[
                [joint_plane_y+2.5,mount_z_min],
                [wall_plane_y+mount_z_min*tan(wall_tilt)-2/cos(wall_tilt),mount_z_min],
                [wall_plane_y+mount_z_max*tan(wall_tilt)-2/cos(wall_tilt),mount_z_max],
                [joint_plane_y+2.5,mount_z_max]
            ]);
}

// Bevel only exposed pad edges; broad central faces carry the mating load.
module joint_pad(rear=true) {
    if (rear) {
        translate([0,(case_d/2-1.5+joint_plane_y-joint_bevel)/2,0])
            cube([joint_w,joint_plane_y-joint_bevel-(case_d/2-1.5),joint_h],center=true);
        hull() {
            translate([0,joint_plane_y-joint_bevel+0.01,0])
                cube([joint_w,0.02,joint_h],center=true);
            translate([0,joint_plane_y-0.01,0])
                cube([joint_w-2*joint_bevel,0.02,joint_h-2*joint_bevel],center=true);
        }
    } else {
        hull() {
            translate([0,joint_plane_y+0.01,0])
                cube([joint_w-2*joint_bevel,0.02,joint_h-2*joint_bevel],center=true);
            translate([0,joint_plane_y+joint_bevel-0.01,0])
                cube([joint_w,0.02,joint_h],center=true);
        }
        translate([0,joint_plane_y+(joint_bevel+3)/2,0])
            cube([joint_w,3-joint_bevel,joint_h],center=true);
    }
}
module rear_joint_pads() {
    for (x=[-mount_rib_x,mount_rib_x]) for (z=joint_zs)
        translate([x,0,z]) {
            joint_pad(true);
            // Offset from optional screw; not a snap-fit or a substitute for welding.
            translate([0,joint_plane_y+joint_pin_depth/2-0.1,-4])
                cube([4,joint_pin_depth+0.2,5],center=true);
        }
}
module mount_joint_pads() {
    for (x=[-mount_rib_x,mount_rib_x]) for (z=joint_zs)
        translate([x,0,z]) joint_pad(false);
}
module joint_sockets() {
    for (x=[-mount_rib_x,mount_rib_x]) for (z=joint_zs)
        translate([x,joint_plane_y+(joint_pin_depth+0.2)/2-0.05,z-4])
            cube([4+2*joint_clearance,joint_pin_depth+0.3,5+2*joint_clearance],center=true);
}
module joint_bolt_holes() {
    for (x=[-mount_rib_x,mount_rib_x])
        translate([x,joint_plane_y,65]) hole_y(joint_screw_d,24);
}

module wall_mount() {
    difference() {
        union() {
            for (x=[-mount_rib_x,mount_rib_x]) wall_mount_rib(x);
            mount_joint_pads();
            wall_frame() {
                // Upper cross plate carries the single screw and joins both webs.
                translate([0,-mount_plate_t/2,mount_plate_z])
                    cube([68,mount_plate_t,34],center=true);
                // Two spaced lower contact pads stabilize pitch and sideways rocking.
                for (x=[-mount_rib_x,mount_rib_x])
                    translate([x,-mount_plate_t/2,mount_pad_z])
                        cube([10,mount_plate_t,mount_pad_h],center=true);
            }
        }
        joint_sockets();
        joint_bolt_holes();
        // Remove the frame envelope plus clearance from all mounting webs.
        // Treat everything below the frame top as occupied: no guessed frame height.
        wall_frame()
            translate([-case_w,-frame_projection-frame_clearance,
                       frame_top_z+frame_clearance-500])
                cube([2*case_w,frame_projection+frame_clearance+100,500]);
        wall_frame() {
            // Genuine gravity keyhole: large head-entry BELOW narrow shaft seat.
            translate([0,-mount_plate_t/2,mount_entry_z])
                hole_y(mount_screw_head_d,mount_plate_t+2);
            hull() {
                translate([0,-mount_plate_t/2,mount_entry_z])
                    hole_y(mount_screw_shank_d,mount_plate_t+2);
                translate([0,-mount_plate_t/2,mount_seat_z])
                    hole_y(mount_screw_shank_d,mount_plate_t+2);
            }
            // Clearance for the head behind the plate along its complete travel.
            hull() {
                for (z=[mount_entry_z,mount_seat_z])
                    translate([0,-mount_plate_t-mount_head_space/2-0.01,z])
                        hole_y(mount_screw_head_d,mount_head_space);
            }
        }
    }
}

module dupont_bay_cut(x=0,z=0, bay_cut_w) {
    translate([x,case_d/2-1.0,z]) cube([bay_cut_w,6,bay_cut_h],center=true);
}

module bus_clip(side=1,z=0) {
    // Open C-style clip for a vertical copper bus. The opening faces the enclosure center,
    // so the bus can be snapped in/out without threading it through closed holes.
    // The clip body bridges back to the rear wall and remains separate from PCB rails.
    x0 = side*bus_x;
    clip_w = 5.8;
    clip_d = 7.2;
    clip_h = 7.0;
    slot_w = bus_wire_d + bus_wire_clearance;
    difference() {
        hull() {
            translate([x0,bus_y,z]) cube([clip_w,clip_d,clip_h],center=true);
            translate([x0,19.0,z]) cube([clip_w,2.0,clip_h],center=true);
        }
        // Vertical bore for copper wire.
        translate([x0,bus_y,z]) cylinder(d=slot_w,h=clip_h+2,center=true);
        // Radial mouth opening toward the center of the enclosure.
        translate([x0-side*2.6,bus_y,z]) cube([4.6,slot_w+0.5,clip_h+2],center=true);
    }
}

module vertical_bus_supports() {
    for (side=[-1,1])
        for (z=bus_clip_zs) bus_clip(side,z);
}

module usb_power_entry_cut() {
    // Bottom-opening rounded rectangular cable feed-through in the REAR half only.
    // Opening to the bottom lets the cable be laid into the shell during service,
    // while the width/height still allow a typical USB plug to pass if required.
    translate([usb_entry_x, case_d/4 + 5.0, usb_entry_z])
        rounded_prism_y(usb_entry_w + usb_entry_clearance, case_d/2 + 8,
                        usb_entry_h + usb_entry_clearance, 2.2);
    // Extend the cut beyond the lower edge so it becomes a clean downward-facing notch.
    translate([usb_entry_x, case_d/4 + 5.0, -96.0])
        cube([usb_entry_w + usb_entry_clearance, case_d/2 + 8, 10], center=true);
}

module usb_strain_relief_bridges() {
    // Two internal zip-tie bridges. Cable runs vertically between them and the rear wall.
    // They are solidly bridged to the rear wall and do not project through the exterior.
    for (sx=[-1,1]) {
        x0 = sx*usb_strain_relief_x;
        difference() {
            hull() {
                translate([x0,15.5,usb_strain_relief_z]) cube([7.0,6.5,10.0],center=true);
                translate([x0,19.0,usb_strain_relief_z]) cube([7.0,2.0,10.0],center=true);
            }
            // Vertical slot for a small cable tie.
            translate([x0,15.2,usb_strain_relief_z])
                cube([usb_zip_tie_slot_w,8.5,usb_zip_tie_slot_h],center=true);
        }
    }
}

module pcb_rails() {
    // Removable ESP32 support: generous 31 mm rail spacing, open ends for serviceability.
    for (x=[-17,17])
        translate([x,14.5,-3]) difference() {
            cube([3.2,11.5,66],center=true);
            translate([x>0?-1.0:1.0,-2.0,0]) cube([2.0,6,60],center=true);
        }
    // small lower stop, leaving USB-C and microSD regions accessible from within when opened
    translate([0,15.0,-35]) cube([31,10.0,3],center=true);
}

module pcb_rails2() {

    // ============================================================
    // PCB DIMENSIONS
    // ============================================================

    // Real width of the ESP32 PCB.
    // The PCB is assumed to be centered on the X axis.
    pcb_width = 28;

    // Extra clearance so the PCB can slide into the rails
    // without being too tight after 3D printing.
    //
    // 0.4 mm total clearance = 0.2 mm on each side.
    pcb_clearance = 0.4;


    // ============================================================
    // RAIL X POSITION
    // ============================================================

    // X position of the center of each rail.
    //
    // PCB width:
    //
    //        -14 mm                  +14 mm
    //           |                       |
    //           |-------- PCB ----------|
    //
    // The slot inside each rail is shifted 1 mm toward the PCB.
    //
    // For a 28 mm PCB:
    //
    //     PCB half width       = 14.0 mm
    //     clearance per side   =  0.2 mm
    //     slot offset          =  1.0 mm
    //
    //     rail_x = 14.0 + 0.2 + 1.0
    //            = 15.2 mm
    //
    rail_x = (pcb_width + pcb_clearance) / 2 + 1;


    // ============================================================
    // RAIL Y POSITION
    // ============================================================

    // Shared with seam_y so the rear rim stays flush with the rails.
    rail_y = pcb_rail_y;


    // ============================================================
    // RAIL Z POSITION
    // ============================================================

    // Vertical position of the rail center.
    //
    // This is unchanged from the original design.
    rail_z = pcb_reference_z;


    // ============================================================
    // RAIL DIMENSIONS
    // ============================================================

    // Thickness of each rail in X.
    rail_width = 3.2;


    // Depth of each rail in Y.
    //
    // ORIGINAL = 20 mm
    // NEW      = 25 mm
    //
    // The additional 5 mm provides more structure around the
    // PCB + Dupont connector assembly.
    rail_depth = pcb_rail_depth;


    // Total vertical length of the rail.
    //
    // This has NOT been changed because the problem is not
    // the vertical PCB height, but the PCB + Dupont depth.
    rail_length = 66;


    // ============================================================
    // PCB SLOT DIMENSIONS
    // ============================================================

    // Width of the PCB slot in X.
    //
    // This should be slightly larger than the PCB thickness.
    slot_width = 2.0;


    // Depth of the PCB slot in Y.
    //
    // This determines how far the PCB edge enters the rail.
    slot_depth = 6;


    // Vertical length of the PCB slot.
    //
    // Slightly shorter than the complete rail so the rail
    // still has material at the upper and lower ends.
    slot_length = pcb_reference_h;


    // ============================================================
    // PCB SLOT X POSITION
    // ============================================================

    // Offset toward the center of the enclosure.
    //
    // RIGHT rail -> slot moves left
    // LEFT rail  -> slot moves right
    //
    slot_x_offset = 1.0;


    // ============================================================
    // PCB SLOT Y POSITION
    // ============================================================

    // Actual channel center: 9.5 - 8.5 = 1.0 mm in global Y.
    slot_y_offset = pcb_slot_y_offset;


    // ============================================================
    // CREATE LEFT AND RIGHT PCB RAILS
    // ============================================================

    // Generate two rails:
    //
    //     -rail_x = left rail
    //     +rail_x = right rail
    //
    for (x = [-rail_x, rail_x])

        // Move each rail to its final position.
        translate([
            x,
            rail_y,
            rail_z
        ])

            // Subtract the PCB slot from the solid rail.
            difference() {

                // =================================================
                // MAIN RAIL BODY
                // =================================================

                // Solid rail.
                //
                // X = rail thickness
                // Y = rail depth
                // Z = rail vertical length
                //
                cube(
                    [
                        rail_width,
                        rail_depth,
                        rail_length
                    ],
                    center = true
                );


                // =================================================
                // PCB SLOT
                // =================================================

                // Move the slot toward the PCB center.
                //
                // Right rail:
                //
                //     x > 0
                //     offset = -1 mm
                //
                // Left rail:
                //
                //     x < 0
                //     offset = +1 mm
                //
                translate([
                    x > 0
                        ? -slot_x_offset
                        :  slot_x_offset,

                    slot_y_offset,

                    0
                ])

                    // Material removed to create the PCB channel.
                    cube(
                        [
                            slot_width,
                            slot_depth,
                            slot_length
                        ],
                        center = true
                    );
            }


    // ============================================================
    // LOWER PCB STOP
    // ============================================================

    // Width of the lower PCB support.
    //
    // PCB = 28 mm
    //
    // 29 mm gives a small amount of support beyond both PCB edges.
    lower_stop_width = 29;


    // Depth of the lower stop.
    //
    // ORIGINAL = 10 mm
    // NEW      = 15 mm
    //
    // This also adds 5 mm toward the Dupont side, matching the
    // modification made to the main rails.
    //
    // Because its center also moves from 11.5 to 14 mm:
    //
    // ORIGINAL:
    //
    //     center = 11.5
    //     depth  = 10
    //     range  = 6.5 ... 16.5
    //
    // NEW:
    //
    //     center = 14
    //     depth  = 15
    //     range  = 6.5 ... 21.5
    //
    // So the original rear edge remains unchanged.
    lower_stop_depth = 15;


    // Thickness of the lower support in Z.
    lower_stop_height = 3;


    // Vertical position of the lower stop.
    //
    // This remains unchanged.
    lower_stop_z = -34.5;


    // ============================================================
    // CREATE LOWER PCB STOP
    // ============================================================

    // The lower stop prevents the ESP32 PCB from sliding downward.
    //
    // Its Y depth has also been extended to better support the
    // PCB + Dupont assembly.
    translate([
        0,
        rail_y,
        lower_stop_z
    ])

        cube(
            [
                lower_stop_width,
                lower_stop_depth,
                lower_stop_height
            ],
            center = true
        );
}

module buzzer_shelf() {
    // Optional SFM-27 shelf envelope ~30x15, held internally with zip-tie slots.
    translate([-20,13,36]) difference() {
        cube([34,15,3],center=true);
        for (x=[-11,11]) translate([x,0,0]) cube([3,16,4],center=true);
    }
}

module rear_shell() {
    difference() {
        union() {
            rear_skin();
            rear_joint_pads();
            rear_catches();
            rear_m3_pads();
            pcb_rails2();
            vertical_bus_supports();
            usb_strain_relief_bridges();
        }
        rear_m3_holes();
        joint_bolt_holes();
        usb_power_entry_cut();
        // Suspension is now in the external wedge, not through the rear skin.
        
        dupont_bay_cut(bay_xs[0],bay_z, bay_cut_w_2pin);
        dupont_bay_cut(bay_xs[1],bay_z, bay_cut_w_8pin);
        dupont_bay_cut(bay_xs[2],bay_z, bay_cut_w_3pin);
    }
}

// ---------------- Electronics reference (NOT printable geometry) ----------------
module electronics_reference() {
    // Illustrative envelope only: excludes pins, USB sockets and ribbon details.
    // Its dimensions follow the old rail assumptions, not a dimensional survey.
    color([0.1,0.45,0.2,0.8])
        translate([0,pcb_reference_y,pcb_reference_z])
            cube([pcb_reference_w,1.6,pcb_reference_h],center=true);
    color([0.15,0.15,0.15,1])
        translate([camera_x,pcb_reference_y-camera_preview_projection/2,camera_z])
            cube([9,camera_preview_projection,9],center=true);
    color([0.25,0.45,0.65,1])
        translate([camera_x,pcb_reference_y-camera_preview_projection,camera_z])
            hole_y(6,1);
}

// ---------------- Modular Dupont inserts ----------------
module snap_tab(zsign=1) {
    // Compliant tab with a small inward barb; use PETG if frequently swapped.
    z0=zsign*(bay_h/2-1.1);
    translate([0,-3.2,z0]) {
        cube([10,4.5,1.6],center=true);
        translate([0,-2.0,zsign*0.8]) cube([10,1.5,2.3],center=true);
    }
}



// ---------------- Assembly visualization ----------------
module assembly() {
    color([0.8,0.8,0.8,0.35]) front_shell();
    color("lightgray") rear_shell();
    color("steelblue") wall_mount();
    if ($preview && show_electronics) %electronics_reference();
}

// Installed view: screw seat at Z=0, wall Y=0, frame top Z=-85.
// Reference wall/frame are preview-only and excluded from exported meshes.
module installed_pose() {
    translate([0,-wall_plane_y*cos(wall_tilt),
               -wall_plane_y*sin(wall_tilt)-mount_seat_z])
        rotate([wall_tilt,0,0]) children();
}
module wall_assembly() {
    installed_pose() assembly();
    if ($preview) {
        %color([0.7,0.65,0.6,0.3])
            translate([0,2,-80]) cube([120,4,320],center=true);
        %color([0.65,0.35,0.15,0.6])
            translate([-60,-frame_projection,-screw_above_frame-150])
                cube([120,frame_projection,150]);
        %color("silver") translate([0,-4,0]) hole_y(mount_screw_shank_d-0.5,10);
        // Red reference plane: vertical collision limit supplied by the user.
        %color([1,0.15,0.1,0.25])
            translate([0,-55,-screw_above_frame-door_vertical_clearance])
                cube([120,120,0.6],center=true);
    }
}

module wall_mount_print() {
    translate([0,0,-joint_plane_y]) rotate([90,0,0]) wall_mount();
}
module exploded() {
    color("lightgray") rear_shell();
    color("steelblue") translate([0,25,0]) wall_mount();
    color("gainsboro") translate([0,-25,0]) front_shell();
}

// ---------------- Part selector ----------------
assert(door_vertical_clearance > door_safety_clearance && door_safety_clearance >= 5,
       "Reserve at least 5 mm of vertical door clearance");
assert(mount_seat_z > 0, "Door clearance requires a different mounting layout");
assert(screw_above_frame > 40, "Insufficient space above frame for this mount");
assert(frame_projection >= 0 && frame_clearance > 0, "Invalid frame clearance");
assert(wall_tilt >= 10 && wall_tilt <= 35, "Wall tilt must be 10..35 degrees downward");
assert(mount_screw_head_d > mount_screw_shank_d+2, "Insufficient screw head retention");
assert(camera_window_w < case_w-2*wall, "Camera window exceeds case width");
assert(abs(camera_z)+camera_window_h/2 < case_h/2-corner_r,
       "Camera window exceeds the straight front area");
if (PART=="front") front_shell();
else if (PART=="rear") rear_shell();
else if (PART=="wall_mount") wall_mount_print();
else if (PART=="exploded") exploded();
else if (PART=="pcb2") pcb_rails2();
else if (PART=="assembly") assembly();
else if (PART=="wall_assembly") wall_assembly();
else assert(false, "Use front, rear, wall_mount, pcb2, assembly, wall_assembly or exploded.");
