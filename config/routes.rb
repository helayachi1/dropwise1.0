Rails.application.routes.draw do
  
  # Define your application routes per the DSL in https://guides.rubyonrails.org/routing.html
Rails.application.routes.draw do
  
  root "home#index"                    # Root path (home page)
  get '/history', to: 'irrigation_events#index', as: 'history' # History page
  get '/new', to: 'irrigation_events#new', as: 'new_irrigation' # New irrigation page
  get '/tasks', to: 'irrigation_events#tasks', as: 'tasks'  # Progress page
  get '/settings', to: 'settings#index', as: 'settings'  # Settings page
  
  resources :irrigation_events do 
    collection do 
      get :tasks
    end
  end

end


  

  # Reveal health status on /up that returns 200 if the app boots with no exceptions, otherwise 500.
  # Can be used by load balancers and uptime monitors to verify that the app is live.
  get "up" => "rails/health#show", as: :rails_health_check

  # Render dynamic PWA files from app/views/pwa/* (remember to link manifest in application.html.erb)
  # get "manifest" => "rails/pwa#manifest", as: :pwa_manifest
  # get "service-worker" => "rails/pwa#service_worker", as: :pwa_service_worker

  # Defines the root path route ("/")
  # root "posts#index"
end
