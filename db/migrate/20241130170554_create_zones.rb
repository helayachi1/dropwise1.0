class CreateZones < ActiveRecord::Migration[8.0]
  def change
    create_table :zones do |t|
      t.string :name
      t.integer :output_1_pin
      t.integer :output_2_pin
      t.string :status

      t.timestamps
    end
  end
end
