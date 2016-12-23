-- Mockups (provavelmente funcoes em C)

callbacks = {}

function driver1_cb(param, cb)
	print("Driver 1", param)
	callbacks[1] = cb
	return 1 -- handle
end

function driver2_cb(param, cb)
	print("Driver 2", param)
	callbacks[2] = cb
	return 2 -- handle
end

function driver2_cancel(handle)
	print("Driver 2 cancel "..handle)
	return true -- retorna sucesso ou falha
end

----------

-- Wrappers
function coroutine_wrap_fg(f, param)
	local context = coroutine.running()
	local cb = function ()
					coroutine.resume(context)
				end
	f(param, cb)
	coroutine.yield()
end

function coroutine_wrap_bg(f, f_cancel, param)
	local state   = "running" -- "cancelled", "done"
	local handle  = nil       -- handle used to cancel / wait
	local context = nil	      -- waiting coroutine
	local cb = function()     -- driver callback
					state = "done"
					if context then
						coroutine.resume(context)
					end
				end
	handle = f(param, cb)
	return {
		cancel = function()
					if state == "done" then
						return false
					elseif state == "running" then
						return f_cancel(handle)
					else
						return true -- already cancelled, but ok
					end
				end,
		wait = function()
					if state == "done" then
						return true
					elseif state == "running" then
						context = coroutine.running()
						coroutine.yield()
						return true -- talvez trocar pelo retorno do yield
									-- para retornar um status do driver
					else
						return false -- was cancelled
					end
				end
	}
end
-----------

-- API
function driver1_fg(param)
	coroutine_wrap_fg(driver1_cb, param)
end

function driver2_bg(param)
	return coroutine_wrap_bg(driver2_cb, driver2_cancel, param)
end

wait_events = coroutine.yield

-- Teste 1
function main()
	while true do
		print("Ini main")
		driver1_fg("")
		print("depois fg")
		h = driver2_bg("")
		print("depois bg")
		h.wait()
		print("depois driver2 wait")
		print("esperando eventos")
		wait_events()
		print("retorno de wait_events")

	end
end

-- Ponto de entrada da aplicacao do usuario
-- coroutine.wrap(main)()

-- while(true) do
--	espera_evento()
-- end

-- Teste 2
function a()
	print("ini a")
	driver1_fg("asd")
	print("-> a")
	driver1_fg("")
	print("fim a")
end

function b()
	print("ini b")
	x = driver2_bg("")
	print("-> b depois do bg")
	x.wait()
	print("-> b depois do wait")
	x = driver2_bg("")
	x.wait()
	print("fim b")
end

