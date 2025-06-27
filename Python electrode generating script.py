import pcbnew, math

board = pcbnew.GetBoard()

# === USER CONFIGURATION ===
SHAPE_FP_NAME = "W3"     # footprint containing multiple copper loops
TP_FP_NAME    = "TP 0.15mm"   # your through-hole testpoint footprint
MAX_NETS      = 16            # supports nets /S1 … /S12
BORDER_MM     = 0.3           # required copper annulus width
PITCH_MM      = 0.55          # hex‐grid pitch for vias
# ===============================

# Build list of net names: ["/S1","/S2",…]
NET_NAMES = [f"/S{i+1}" for i in range(MAX_NETS)]

def pip(x, y, pts):
    inside = False
    for i in range(len(pts) - 1):
        x1, y1 = pts[i].x, pts[i].y
        x2, y2 = pts[i+1].x, pts[i+1].y
        if (y1 > y) != (y2 > y):
            xin = (x2 - x1) * (y - y1) / ( (y2 - y1) + 1e-9 ) + x1
            if x < xin:
                inside = not inside
    return inside

def seg_dist(x, y, x1, y1, x2, y2):
    dx, dy = x2 - x1, y2 - y1
    if dx == 0 and dy == 0:
        return math.hypot(x - x1, y - y1)
    t = ((x - x1)*dx + (y - y1)*dy) / (dx*dx + dy*dy)
    t = max(0, min(1, t))
    px, py = x1 + t*dx, y1 + t*dy
    return math.hypot(x - px, y - py)

# Lookup our footprints
shape_fp_list = [f for f in board.GetFootprints()
                 if f.GetFPID().GetLibItemName() == SHAPE_FP_NAME]
tp_tpl = next((f for f in board.GetFootprints()
               if f.GetFPID().GetLibItemName() == TP_FP_NAME), None)
if not shape_fp_list or not tp_tpl:
    raise RuntimeError("Required footprints not found on board.")

pitch = pcbnew.FromMM(PITCH_MM)
dy    = pitch * math.sqrt(3) / 2
border = pcbnew.FromMM(BORDER_MM)
total_vias = 0

for shape_fp in shape_fp_list:
    # 1) Gather all closed F.Cu loops in this footprint
    loops = []
    for gi in shape_fp.GraphicalItems():
        if gi.GetLayer() == pcbnew.F_Cu:
            try:
                ps = gi.GetPolyShape()
                for i in range(ps.OutlineCount()):
                    o = ps.Outline(i)
                    pts = [o.GetPoint(j) for j in range(o.PointCount())]
                    if pts[0] != pts[-1]:
                        pts.append(pts[0])
                    loops.append(pts)
            except:
                pass

    if not loops:
        continue

    # 2) Process each loop as its own electrode

# Sort loops by leftmost X (for consistent net order L→R)
    loops.sort(key=lambda pts: min(p.x for p in pts))
    for idx, pts in enumerate(loops):
        if idx >= len(NET_NAMES):
            break
        netname = NET_NAMES[idx]
        net     = board.FindNet(netname)
        if not net:
            raise RuntimeError(f"Net '{netname}' not found on board.")
        netcode = net.GetNetCode()

        # Compute bounding box
        xs = [p.x for p in pts]; ys = [p.y for p in pts]
        xmin, xmax = min(xs), max(xs)
        ymin, ymax = min(ys), max(ys)

        # 2a) Place vias (must be ≥border from any edge)
        placed = 0
        y = ymin; row = 0
        while y <= ymax:
            xoff = 0 if (row % 2) == 0 else pitch/2
            x = xmin + xoff
            while x <= xmax:
                if pip(x, y, pts):
                    dmin = min(
                        seg_dist(x, y, pts[i].x, pts[i].y, pts[i+1].x, pts[i+1].y)
                        for i in range(len(pts)-1)
                    )
                    if dmin >= border:
                        via = tp_tpl.Duplicate()
                        via.SetPosition(pcbnew.VECTOR2I(int(x), int(y)))
                        # **Only** override net; leave pad layer set alone!
                        for pad in via.Pads():
                            pad.SetNetCode(netcode)
                        board.Add(via)
                        placed += 1
                        total_vias += 1
                x += pitch
            y += dy; row += 1

        print(f"• Loop {idx+1}→{netname}: placed {placed} vias")

        # 2b) Create the F.Cu zone (outline only; you’ll press B to fill)
        zone = pcbnew.ZONE(board)
        zone.SetLayer(pcbnew.F_Cu)
        zone.SetNetCode(netcode)
        zone.SetIsFilled(True)
        zone.SetPadConnection(pcbnew.ZONE_CONNECTION_FULL)
        zone.SetLocalClearance(0)

        chain = pcbnew.SHAPE_LINE_CHAIN()
        for p in pts:
            chain.Append(p)
        zone.Outline().AddOutline(chain)
        board.Add(zone)

    # 3) Remove the footprint’s own F.Cu graphics
    to_remove = [gi for gi in shape_fp.GraphicalItems()
                 if gi.GetLayer() == pcbnew.F_Cu]
    for gi in to_remove:
        shape_fp.Remove(gi)

# 4) Finally, refresh (zones are present unfilled; hit “B” to pour)
pcbnew.Refresh()
print(f"\n✅ Done. {total_vias} vias placed; zones created—press B to fill.")  
