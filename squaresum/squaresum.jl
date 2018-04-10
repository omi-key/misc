using JSON
js = JSON.parse(open("results.json"))
d = Dict{String,Float64}()
for sub in js
    if haskey(d,sub["problem_id"])
        d[sub["problem_id"]] = max(d[sub["problem_id"]], sub["point"])
    else
        d[sub["problem_id"]] = sub["point"]
    end
end
sqsm = 0.0
for i in values(d)
    if i > 3000
        continue
    end
    sqsm += (i/100)^2
end
println(sqsm)
