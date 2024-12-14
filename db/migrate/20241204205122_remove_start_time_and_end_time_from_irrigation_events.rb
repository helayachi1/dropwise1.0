class RemoveStartTimeAndEndTimeFromIrrigationEvents < ActiveRecord::Migration[8.0]
  def change
    remove_column :irrigation_events, :start_time, :datetime
    remove_column :irrigation_events, :end_time, :datetime
  end
end
