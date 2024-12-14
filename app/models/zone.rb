class Zone < ApplicationRecord
	has_many :irrigation_events, dependent: :destroy
	validates :name, presence: true 
	validates :output_1_pin, presence: true, numericality: { only_integer: true}
	validates :output_2_pin, presence: true, numericality: { only_integer: true}
end
