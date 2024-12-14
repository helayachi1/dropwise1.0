class IrrigationEventsController < ApplicationController
  
  def index
    @irrigation_events = IrrigationEvent.all
  end

  def tasks 
    current_time = Time.current

# Fetch all events from the database
all_events = IrrigationEvent.all

# Filter ongoing events: started but not yet finished
@ongoing_events = all_events.select do |event|
  event.created_at <= current_time && event.created_at + event.duration.minutes > current_time
end

# Filter completed events: finished before the current time
@completed_events = all_events.select do |event|
  event.created_at + event.duration.minutes <= current_time
end

  end

  def new
    @irrigation_event = IrrigationEvent.new
  end

  def create
    @irrigation_event = IrrigationEvent.new(event_params)

    if @irrigation_event.save
      redirect_to tasks_path
    else
      render :new, status: :unprocessable_entity
    end
  end

  private
  
  def event_params
      params.expect(irrigation_event: [:zone_id, :duration])
  end

end
