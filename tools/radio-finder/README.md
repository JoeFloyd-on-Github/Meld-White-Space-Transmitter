# tools/radio-finder — AM & FM radio stations by major market

A dependency-free browser tool (sibling to [`tools/whitespace-finder/`](../whitespace-finder/))
that lists licensed US **AM** (540–1700 kHz) and **FM** (88.1–107.9 MHz)
broadcast radio stations for any major market or coordinates, using public FCC
records mirrored in [`data/`](data).

## Run it

Serve the folder over HTTP (browsers block `fetch()` over `file://`):

```bash
cd tools/radio-finder
python3 -m http.server 8001
```

Then open <http://localhost:8001/>. Pick a **market from the dropdown** (top 5
per state/territory, clustered by station proximity and centered on the cluster
centroid), enter coordinates, or click **Use my location**.

## Features

- Filter by **band** (All, FM only, AM only), **radius** (default 80 km), and
  live text search (callsign, city, licensee, or frequency).
- Sort by **frequency** (AM then FM), **distance**, or **power**.
- Shows daytime/nighttime power (kW) for AM and ERP (kW) + HAAT (m) for FM.

## Data

[`data/`](data) holds a **2026-11-22** snapshot of US licensed stations (`LIC`)
exported from the FCC Media Bureau query endpoints
(`https://transition.fcc.gov/fcc-bin/amq?country=US&status=3&list=4` and
`https://transition.fcc.gov/fcc-bin/fmq?country=US&status=3&serv=FM&list=4`),
converted to CSV:

- [`data/fcc_am_station.csv`](data/fcc_am_station.csv) — 4,267 AM stations (collapsed to one row per `facility_id` with `day_kw` / `night_kw`)
- [`data/fcc_fm_station.csv`](data/fcc_fm_station.csv) — 11,414 FM stations
