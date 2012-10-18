Pod::Spec.new do |s|
  s.name         = "Chipmunk-Physics"
  s.version      = "6.1.1"
  s.summary      = "A fast and lightweight 2D game physics library."
  s.homepage     = "http://chipmunk-physics.net"
  s.license      = { :type => 'MIT' }
  s.author       = { "slembcke" => "email@address.com" }
  s.source       = { :git => "https://github.com/slembcke/Chipmunk-Physics.git", :tag => "Chipmunk-6.1.1" }
  s.source_files = 'src', 'src/constraints', 'include/chipmunk', 'include/chipmunk', 'include/chipmunk/constraints'
  s.public_header_files = 'include'
  s.header_mappings_dir = 'include'
  s.framework  = 'Foundation'
end
