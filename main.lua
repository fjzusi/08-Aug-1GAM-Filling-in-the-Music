require "lib/middleclass"

SCREEN_WIDTH = 400
SCREEN_HEIGHT = 400

function love.load()
	img = love.graphics.newImage("asset/image/TitleScreen.png");
end

function love.keypressed(key, unicode)
	if key == "escape" then
		love.event.push("quit")
	end
end

function love.keyreleased(key, unicode)
	
end

function love.update(dt)
	
end

function love.draw()
	love.graphics.draw(img, 0, 0)
end