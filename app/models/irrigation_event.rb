class IrrigationEvent < ApplicationRecord
  belongs_to :zone
  validates :zone_id, presence: true
  validates :duration, presence: true, numericality: { greater_than: 0 }
end
