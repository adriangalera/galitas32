// DIY ESP32 Alarm / Photo-Detector Enclosure
// Parametric OpenSCAD source, dimensions in mm.
// Designed for 0.4 mm nozzle / 0.2 mm layers, PLA or PETG.
$fn = 48;

PART = "assembly"; // front, rear, camera_cradle, dupont_2, dupont_3, dupont_4, dupont_6, angle_gauge, assembly

// ---------------- User-tunable parameters ----------------
case_w = 74;
case_d = 44;
case_h = 190;
corner_r = 11;
wall = 2.4;
seam_y = 0;

camera_z = 48;
camera_pivot_y = -5.8;
camera_module_w = 24.5;    // generous OV2640 PCB envelope
camera_module_h = 24.5;
camera_cradle_w = 29.0;
camera_cradle_h = 30.0;
pivot_clearance_d = 2.9;   // M2.5 bolt clearance
lock_clearance_d = 2.9;
lock_radius = 10.5;
cam_min_angle = 10;
cam_nom_angle = 20;
cam_max_angle = 35;

m3_screw_clearance = 3.4;
m3_insert_pilot_d = 4.2;   // intentionally easy to tune for your actual heat-set insert
m3_boss_d = 9.5;
m3_z = -71;
m3_x = 20;

bay_w = 18;
bay_h = 10;
bay_cut_w = 17.8;
bay_cut_h = 9.8;
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
        translate([0,-case_d/4-0.1,0]) cube([case_w+4,case_d/2+0.2,case_h+4],center=true);
    }
}

module rear_skin() {
    intersection() {
        shell_skin();
        translate([0,case_d/4+0.1,0]) cube([case_w+4,case_d/2+0.2,case_h+4],center=true);
    }
}

// ---------------- Front shell internal mechanics ----------------
module camera_aperture_cut() {
    // Wide/tall rounded opening avoids a restrictive circular tunnel.
    translate([0,-case_d/2+0.5,camera_z])
        rounded_prism_y(22,8,42,3.5);
}

module arc_slot_x(r=lock_radius, a0=cam_min_angle, a1=cam_max_angle, d=3.2, len=8) {
    // Slot lies in Y-Z plane, axis through X.
    for (a=[a0:2:a1-2]) hull() {
        translate([0, r*cos(a), r*sin(a)]) hole_x(d,len);
        translate([0, r*cos(a+2), r*sin(a+2)]) hole_x(d,len);
    }
}

module camera_supports() {
    // Fixed ears and ribs remain well behind the front inner surface.
    support_x = camera_cradle_w/2 + 2.2;
    ear_t = 4.0;
    ear_y = camera_pivot_y;
    ear_z = camera_z;
    for (sx=[-1,1]) {
        x0=sx*support_x;
        difference() {
            union() {
                translate([x0,ear_y,ear_z]) rotate([0,90,0]) cylinder(d=13,h=ear_t,center=true);
                // Left side only: fixed locking plate covering the 10-35 degree arc.
                if (sx < 0) hull() {
                    translate([x0, ear_y+lock_radius*cos(cam_min_angle), ear_z+lock_radius*sin(cam_min_angle)]) rotate([0,90,0]) cylinder(d=8,h=ear_t,center=true);
                    translate([x0, ear_y+lock_radius*cos(cam_max_angle), ear_z+lock_radius*sin(cam_max_angle)]) rotate([0,90,0]) cylinder(d=8,h=ear_t,center=true);
                    translate([x0,ear_y,ear_z]) rotate([0,90,0]) cylinder(d=8,h=ear_t,center=true);
                }
                // printable rib back to the front wall, entirely internal
                hull() {
                    translate([x0, -18.8, ear_z+2]) cube([ear_t,3.0,11],center=true);
                    translate([x0, ear_y, ear_z]) cube([ear_t,4,9],center=true);
                }
            }
            translate([x0,ear_y,ear_z]) hole_x(pivot_clearance_d,ear_t+2);
            // Only LEFT side has the curved locking slot.
            if (sx < 0)
                translate([x0,ear_y,ear_z]) arc_slot_x(lock_radius,cam_min_angle,cam_max_angle,lock_clearance_d+0.35,ear_t+2);
        }
    }
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
    // Two real seam-crossing hooks. Front half extends through Y=0 into rear catch volume.
    for (x=[-20,20]) {
        union() {
            translate([x,-3.0,76]) cube([8,6,4],center=true);
            translate([x,1.25,78.2]) cube([8,2.8,4.4],center=true); // barb across seam
            hull() {
                translate([x,-18.6,74]) cube([8,3.0,4],center=true);
                translate([x,-3.0,76]) cube([8,2,4],center=true);
            }
        }
    }
}

module front_shell() {
    difference() {
        union() {
            front_skin();
            camera_supports();
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
                translate([x,3.1,80.6]) cube([11,6.2,3.4],center=true);
                translate([x-4.8,3.1,77.8]) cube([1.8,6.2,7.5],center=true);
                translate([x+4.8,3.1,77.8]) cube([1.8,6.2,7.5],center=true);
            }
            translate([x,1.2,78.1]) cube([9.4,4.0,4.8],center=true); // receiving pocket
        }
    // rear-wall bridges keep catches integral while leaving hook pocket open
    for (x=[-20,20]) hull() {
        translate([x,5.5,82.0]) cube([8,2,2.4],center=true);
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

module keyhole_cut(z=40) {
    translate([0,case_d/2-0.2,z]) {
        hole_y(4.4,7);
        translate([0,0,5]) hull() {
            hole_y(4.4,7);
            translate([0,0,5]) hole_y(7.5,7);
        }
    }
}

module anti_rotation_hole(z=43) {
    // Small secondary wall screw / locating hole below the main keyhole.
    translate([0,case_d/2-0.2,z]) hole_y(4.0,7);
}

module dupont_bay_cut(x=0,z=0) {
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
            rear_catches();
            rear_m3_pads();
            pcb_rails();
            vertical_bus_supports();
            usb_strain_relief_bridges();
        }
        rear_m3_holes();
        usb_power_entry_cut();
        keyhole_cut(55);
        anti_rotation_hole(43);
        for (i=[0:len(bay_xs)-1]) dupont_bay_cut(bay_xs[i],bay_z);
    }
}

// ---------------- Adjustable camera cradle ----------------
module camera_cradle() {
    difference() {
        union() {
            // Main camera plate/frame, front face toward -Y at zero angle.
            translate([0,-3.0,0]) cube([camera_cradle_w,4.0,camera_cradle_h],center=true);
            // Side retaining rails for camera PCB; open top for removal.
            for (x=[-(camera_module_w/2+1.2),(camera_module_w/2+1.2)])
                translate([x,-0.2,-1.0]) cube([2.2,7.5,camera_module_h+2],center=true);
            translate([0,-0.2,-(camera_module_h/2+1.2)]) cube([camera_module_w+4,7.5,2.2],center=true);
            // Pivot lugs inside fixed supports.
            for (x=[-(camera_cradle_w/2-1.4),(camera_cradle_w/2-1.4)])
                translate([x,0,0]) rotate([0,90,0]) cylinder(d=9,h=2.8,center=true);
            // One-sided locking lug on left only, structurally bridged to the pivot lug.
            translate([-(camera_cradle_w/2-1.4), lock_radius, 0]) rotate([0,90,0]) cylinder(d=7,h=2.8,center=true);
            hull() {
                translate([-(camera_cradle_w/2-1.4), 2.0, 0]) rotate([0,90,0]) cylinder(d=5.5,h=2.8,center=true);
                translate([-(camera_cradle_w/2-1.4), lock_radius-2.0, 0]) rotate([0,90,0]) cylinder(d=5.5,h=2.8,center=true);
            }
            // Ribbon cable guide tab, rear/lower side.
            translate([8,1.8,-11]) cube([8,3,3],center=true);
        }
        // Lens window in cradle itself.
        translate([0,-3.0,0]) cube([16.5,7,16.5],center=true);
        // Coaxial M2.5 pivot clearance through both lugs and center.
        hole_x(pivot_clearance_d,camera_cradle_w+5);
        // Lock hole only on left lug; located at +Y radius and follows rotation.
        translate([-(camera_cradle_w/2-1.4), lock_radius, 0]) hole_x(lock_clearance_d,5);
        // Ribbon path opening.
        translate([7,-2,-10]) cube([9,8,5],center=true);
    }
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

module dupont_insert(n=4) {
    difference() {
        union() {
            // External flange bears against rear shell so plug-in force cannot push insert inward.
            cube([20.5,2.2,12.5],center=true);
            translate([0,-2.8,0]) cube([17.3,4.0,9.3],center=true);
            snap_tab(1);
            snap_tab(-1);
        }
        for (i=[0:n-1]) {
            x=(i-(n-1)/2)*header_pitch;
            translate([x,0,0]) cube([header_hole,10,header_hole],center=true);
        }
    }
}

// ---------------- Camera angle setup gauge ----------------
module angle_gauge() {
    // Small 32 x 18 x 2.4 setup tool; three edges correspond to 10/20/35 deg.
    linear_extrude(height=2.4,center=true)
    polygon(points=[[0,0],[30,0],[30,5.3],[18,5.3],[18,11.0],[8,11.0],[8,17.2],[0,17.2]]);
}

// ---------------- Assembly visualization ----------------
module camera_at(angle=cam_nom_angle) {
    translate([0,camera_pivot_y,camera_z]) rotate([angle,0,0]) camera_cradle();
}

module assembly() {
    color("gainsboro") front_shell();
    color("lightgray") rear_shell();
    color("orange") camera_at(cam_nom_angle);
    // Example inserts occupying the 3 rear bays: 3,4,6 pin.
    for (j=[0:2]) {
        n=[3,4,6][j];
        translate([bay_xs[j],case_d/2+0.9,bay_z]) rotate([0,0,0]) dupont_insert(n);
    }
}

// ---------------- Part selector ----------------
if (PART=="front") front_shell();
else if (PART=="rear") rear_shell();
else if (PART=="camera_cradle") camera_cradle();
else if (PART=="dupont_2") dupont_insert(2);
else if (PART=="dupont_3") dupont_insert(3);
else if (PART=="dupont_4") dupont_insert(4);
else if (PART=="dupont_6") dupont_insert(6);
else if (PART=="angle_gauge") angle_gauge();
else assembly();
