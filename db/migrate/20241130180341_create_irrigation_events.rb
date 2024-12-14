class CreateIrrigationEvents < ActiveRecord::Migration[8.0]
  def change
    create_table :irrigation_events do |t|
      t.references :zone, null: false, foreign_key: true
      t.integer :duration

      t.timestamps
    end
  end
end
