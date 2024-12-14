# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# This file is the source Rails uses to define your schema when running `bin/rails
# db:schema:load`. When creating a new database, `bin/rails db:schema:load` tends to
# be faster and is potentially less error prone than running all of your
# migrations from scratch. Old migrations may fail to apply correctly if those
# migrations use external dependencies or application code.
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema[8.0].define(version: 2024_12_06_111723) do
  create_table "irrigation_events", force: :cascade do |t|
    t.integer "zone_id", null: false
    t.integer "duration"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.index ["zone_id"], name: "index_irrigation_events_on_zone_id"
  end

  create_table "sensor_readings", force: :cascade do |t|
    t.float "temperature"
    t.float "humidity"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.float "water_level"
  end

  create_table "zones", force: :cascade do |t|
    t.string "name"
    t.integer "output_1_pin"
    t.integer "output_2_pin"
    t.string "status"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
  end

  add_foreign_key "irrigation_events", "zones"
end
